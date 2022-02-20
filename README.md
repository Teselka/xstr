## Example
```cpp
auto str = _x("my awesome string!");
```


<details>
  <summary>IDA 7.6 decompiled</summary>
  
  ```cpp
  v3 = &v6;
  v6 = _xmm;
  v7 = 12663;
  v4 = 0;
  v8 = 16;
  do
  {
    *(_BYTE *)v3 ^= 0x10u;
    v3 = (__int128 *)((char *)v3 + 1);
    ++v4;
  }
  while ( v4 < 0x13 );
  ```
</details>


## Information
Compile-time string encryption.

Requires at least -O1 flag and C++ 14

Supports GCC, MSVC, CLANG compilers.

For MSVC and C++ 20 recommended to use ```/Zc:__cplusplus ```.

