<p align="center">  
    <img src="https://tools.etdon.com/placeholder-image/generate?width=830&height=207&background-color=3E5879,213555&text=fiddler-customizer&text-color=FFFFFF" width=830 height=207>    
</p>

<div align="center">

![Windows](https://img.shields.io/badge/Windows-white?style=for-the-badge&logo=github&label=Platform&color=%230173b3)
![x64](https://img.shields.io/badge/x64-white?style=for-the-badge&logo=mingww64&label=Architecture&color=%233fb911)
![C++/CLI](https://img.shields.io/badge/C%2B%2B%2FCLI-white?style=for-the-badge&logo=c%2B%2B&label=Language&color=%2300599C)
![Apache 2.0](https://img.shields.io/badge/Apache%202.0-white?style=for-the-badge&logo=apache&label=License&color=%23D22128)

</div>

## 🔰 Introduction

The `fiddler-customization` project is a proof-of-concept tool to apply a dark mode look to the [Fiddler Classic](https://www.telerik.com/fiddler/fiddler-classic) UI which doesn't support it by default and is using
various, deprecated, WinForm controls making customization tedious. The tool works by leveraging mostly un-documented Windows API for drawing/flagging as well as C++/CLI to interact with the CIL components of the application
during run-time since Fiddler Classic is written in C#.

As mentioned this is mainly a proof-of-concept and the project is not finished. Some deprecated controls will still have white borders, dark mode rendering might not apply to all windows and pages and the visibility of some
controls might not be optimal. It's also a proof-of-concept because none of the used techniques fully depend on the Fiddler Classic implementation, with rather small adjustments the same effect can be achieved for other
WinForm applications.

Preview:
<p align="center">  
    <img src="https://i.imgur.com/Ao2jq36.png">    
</p>

Credits to the [win32-custom-menubar-aero-theme](https://github.com/adzm/win32-custom-menubar-aero-theme) project for providing additional details regarding the mostly un-documented drawing process of native themes.

## 🚀 Getting Started

> [!IMPORTANT]
> Requirements:
> - Any DLL Injector
>   - Recommended: [loadlib-dll-injector](https://github.com/etdon/loadlib-dll-injector)

If want to download a pre-compiled executable you can find all releases paired with their checksums here: [Releases](https://github.com/etdon/fiddler-customizer/releases)

In order to inject the DLL into Fiddler Classic using the recommended DLL injector you can use the following command:
```
dll-injector.exe "Fiddler.exe" <DLL path>
```
## 🫴 Contributing
The contribution guidelines are a part of the `shared-guidelines` repository and can be found here: [Contributing][contributing]

## 📄 License
The `fiddler-customization` project is licensed under the [Apache 2.0 License][license].
```
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

[contributing]: https://github.com/etdon/shared-guidelines/blob/main/CONTRIBUTING.md
[license]: https://github.com/etdon/fiddler-customization/blob/master/LICENSE
