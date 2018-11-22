#include "DX12Core.h"
#include "CommandListManager.h"
#include "ColorBuffer.h"
#include "Helpers.h"
#include "Display.h"

namespace Bunny {
  namespace Graphics {
    namespace DX12 {

      namespace Core {
        
        //cpp definitione
        Display theDisplay_(1280,720);
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;

        void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
        {
          Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
          *ppAdapter = nullptr;

          SIZE_T MaxSize = 0;

          for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
          {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
              // Don't select the Basic Render Driver adapter.
              // If you want a software adapter, pass in "/warp" on the command line.
              continue;
            }

            // Check to see if the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (desc.DedicatedVideoMemory > MaxSize && SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
              BN_CORE_INFO("D3D12-capable hardware found: {0} ({1} MB)", Bunny::Helpers::ToCharStar(L"TEST TEST"), desc.DedicatedVideoMemory >> 20);
              MaxSize = desc.DedicatedVideoMemory;
            }
          }

          *ppAdapter = adapter.Detach();
        }

        void CreateInfoQueue()
        {
          ID3D12InfoQueue* pInfoQueue = nullptr;
          if (SUCCEEDED(g_Device->QueryInterface(IID_PPV_ARGS(&pInfoQueue))))
          {
            // Suppress whole categories of messages
            //D3D12_MESSAGE_CATEGORY Categories[] = {};

            // Suppress messages based on their severity level
            D3D12_MESSAGE_SEVERITY Severities[] =
            {
              D3D12_MESSAGE_SEVERITY_INFO
            };

            // Suppress individual messages by their ID
            D3D12_MESSAGE_ID DenyIds[] =
            {
              // This occurs when there are uninitialized descriptors in a descriptor table, even when a
              // shader does not access the missing descriptors.  I find this is common when switching
              // shader permutations and not wanting to change much code to reorder resources.
              D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,

              // Triggered when a shader does not export all color components of a render target, such as
              // when only writing RGB to an R10G10B10A2 buffer, ignoring alpha.
              D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_PS_OUTPUT_RT_OUTPUT_MISMATCH,

              // This occurs when a descriptor table is unbound even when a shader does not access the missing
              // descriptors.  This is common with a root signature shared between disparate shaders that
              // don't all need the same types of resources.
              D3D12_MESSAGE_ID_COMMAND_LIST_DESCRIPTOR_TABLE_NOT_SET,

              // RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS
              (D3D12_MESSAGE_ID)1008,
            };

            D3D12_INFO_QUEUE_FILTER NewFilter = {};
            //NewFilter.DenyList.NumCategories = _countof(Categories);
            //NewFilter.DenyList.pCategoryList = Categories;
            NewFilter.DenyList.NumSeverities = _countof(Severities);
            NewFilter.DenyList.pSeverityList = Severities;
            NewFilter.DenyList.NumIDs = _countof(DenyIds);
            NewFilter.DenyList.pIDList = DenyIds;

            pInfoQueue->PushStorageFilter(&NewFilter);
            pInfoQueue->Release();
          }
        }

        void CheckTypedLoadUavSupport()
        {
          D3D12_FEATURE_DATA_D3D12_OPTIONS FeatureData = {};
          if (SUCCEEDED(g_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &FeatureData, sizeof(FeatureData))))
          {
            if (FeatureData.TypedUAVLoadAdditionalFormats)
            {
              D3D12_FEATURE_DATA_FORMAT_SUPPORT Support =
              {
                DXGI_FORMAT_R11G11B10_FLOAT, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE
              };

              if (SUCCEEDED(g_Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
                (Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
              {
                //m_bTypedUAVLoadSupport_R11G11B10_FLOAT = true;
              }

              Support.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

              if (SUCCEEDED(g_Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
                (Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
              {
                //m_bTypedUAVLoadSupport_R11G11B10_FLOAT = true;
              }
            }
          }
        }

        //h definitions
        ID3D12Device* g_Device = nullptr;
        CommandListManager g_CommandManager;

        DescriptorAllocator g_DescriptorAllocator[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] =
        {
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
        };

        void Init(void) {
          UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
          // Enable the debug layer (requires the Graphics Tools "optional feature").
          // NOTE: Enabling the debug layer after device creation will invalidate the active device.
          {
            Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
            {
              debugController->EnableDebugLayer();

              // Enable additional debug layers.
              dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            }
          }
#endif

          // factory
          Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
          Helpers::ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

          // device
          Microsoft::WRL::ComPtr<ID3D12Device> pDevice;
          if (false)
          {
            Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
            Helpers::ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

            Helpers::ThrowIfFailed(D3D12CreateDevice(
              warpAdapter.Get(),
              D3D_FEATURE_LEVEL_11_0,
              IID_PPV_ARGS(&pDevice)
            ));
          }
          else
          {
            Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
            GetHardwareAdapter(factory.Get(), &hardwareAdapter);

            Helpers::ThrowIfFailed(D3D12CreateDevice(
              hardwareAdapter.Get(),
              D3D_FEATURE_LEVEL_11_0,
              IID_PPV_ARGS(&pDevice)
            ));
          }

          Bunny::Graphics::DX12::Core::g_Device = pDevice.Get();

          // info queue
#if _DEBUG
          CreateInfoQueue();
#endif

          CheckTypedLoadUavSupport();

          // command queue
          Bunny::Graphics::DX12::Core::g_CommandManager.Create(g_Device);

          // swap chain and render target views an RTV descriptor heap is in the display
          theDisplay_.Init(factory, pDevice, Bunny::Platform::GetMainPlatformWindow());

          Helpers::ThrowIfFailed(g_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
        }
      }
    }
  }
}
