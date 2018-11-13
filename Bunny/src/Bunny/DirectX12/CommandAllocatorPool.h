#pragma once

#include "stdafx.h"

class CommandAllocatorPool
{
public:
  CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE Type);
  ~CommandAllocatorPool();
};

