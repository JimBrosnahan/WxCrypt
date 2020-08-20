# WxWidget + Crypto++ demo application #

A VS2019 example solution using WxWidgets GUI library with ciphering from Crypto++ library. Cipering is based on 128 bit AES-CFB Mode.


## Prerequisite ##

Clone both WxWidgets and CrytoPP GitHub respositories placing at same mount point as MyWxApp clone repo.

https://github.com/wxWidgets/wxWidgets  
select git tag v3.0.5  
> git checkout v3.0.5

https://github.com/weidai11/cryptopp  
select git tag CRYPTOPP_8_2_0  
>git checkout CRYPTOPP_8_2_0

Note: WxWidget Stable Release 3.0.5 also works too.

## Build Notes ##
- Before building wxWidgets libraries, copy (or MKLINK) include/wx/msw/setup0.h to setup.h. See *Upgrading Existing Git Checkout** in docs/msw/install.md
- Build WxWidgets solution separately using the wx_vc12.sln, creating static wxwidget libraries. (.lib)
- Load MyWxApp.sln solution file
- Select cryptlib project properties.
- In C/C++ code generation -> Runtime Library, select MultiThreaded Debug DLL

### Future Plans ###
- Include 256 bit keys selection drop-down
- Include "pre-defined" IV selection drop-down
- Remove Multi-Thread DLL runtime dependency from cryptolib.
- Rebuild with more recent WxWidgets release tag


