## Example
```cpp
auto str = _x("my awesome string!");
```
  
## Output
<details>
  <summary>IDA 7.6 decompiled</summary>

  ```cpp
  v11 = -2136872978;
  v3 = &v6;
  v10 = -369953549;
  v9 = -1595544081;
  v8 = -203032607;
  v7 = -24327;
  v6 = -19;
  v12 = 0;
  do
  {
    *(_DWORD *)v3 ^= 0x80808080;
    v3 += 4;
  }
  while ( v12 < 4 );
  v4 = v3 - &v6;
  do
  {
    *v3++ ^= 0x80u;
    ++v4;
  }
  while ( v4 < 0x13 );
  ```
</details>
  
<details>
  <summary>IDA 7.6 decompiled (no optimization)</summary>
  
  ```cpp
  v8 = -254699618;
  v0 = &v3;
  v7 = -1719499645;
  v1 = 0;
  v6 = -795501153;
  v5 = -2087352431;
  v4 = -12151;
  v3 = -99;
  do
  {
    *v0++ ^= 0xF0u;
    ++v1;
  }
  while ( v1 < 0x13 );
  ```
</details>
  
<details>
  <summary>Ghidra decompiled</summary>

  ```cpp
  local_c = 0xb091d7de;
  puVar1 = (uint *)&local_1b;
  local_10 = 0xd9c2c4c3;
  local_14 = 0x90d5dddf;
  local_18 = 0xc3d5c7d1;
  local_1a = 0x90c9;
  local_1b = 0xdd;
  local_8 = 0;
  do {
    *puVar1 = *puVar1 ^ 0xb0b0b0b0;
    puVar1 = puVar1 + 1;
  } while (local_8 < 4);
  uVar2 = (int)puVar1 - (int)&local_1b;

  do {
    *(byte *)puVar1 = *(byte *)puVar1 ^ 0xb0;
    puVar1 = (uint *)((int)puVar1 + 1);
    uVar2 = uVar2 + 1;
  } while (uVar2 < 0x13);
  ```
</details>

<details>
  <summary>Ghidra decompiled (no optimization)</summary>

  ```cpp
  local_c = 0x3011575e;
  pbVar1 = &local_1b;
  local_10 = 0x59424443;
  uVar2 = 0;
  local_14 = 0x10555d5f;
  local_18 = 0x43554751;
  local_1a = 0x1049;
  local_1b = 0x5d;
  do {
    *pbVar1 = *pbVar1 ^ 0x30;
    pbVar1 = pbVar1 + 1;
    uVar2 = uVar2 + 1;
  } while (uVar2 < 0x13);
  ```
</details>
  
## Preprocessor
`XORSTR_DISABLE_OPTIMIZATION` - disables optimization, used to make output smaller

## Information
Compile-time string encryption.

Requires at least -O1 flag and C++ 14

Supports GCC, MSVC, CLANG compilers.

For MSVC and C++ 20 recommended to use ```/Zc:__cplusplus ```.

