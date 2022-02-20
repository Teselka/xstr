## Example
```cpp
auto str = _x("my awesome string!");
```
  
<details>
    <summary>IDA 7.6 decompiled</summary>
  
    ```cpp
    v3 = v8;
    qmemcpy(v8, "=)p1'5#?=5p#$\"9>7qP", sizeof(v8));
    v4 = 5;
    do
    {
      *v3++ ^= 0x50505050u;
      --v4;
    }
    while ( v4 );
    v5 = &v9;
    v6 = 20;
    do
    {
      *v5++ ^= 0x50u;
      ++v6;
    }
    while ( v6 < 0x13 );
    ```
  
</details>
  
<details>
  <summary>IDA 7.6 decompiled (no big string optimization)</summary>
  
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

## Preprocessor
`XORSTR_DISABLE_BIGSTR_OPT` - disables big string optimization, used to make output smaller

## Information
Compile-time string encryption.

Requires at least -O1 flag and C++ 14

Supports GCC, MSVC, CLANG compilers.

For MSVC and C++ 20 recommended to use ```/Zc:__cplusplus ```.

