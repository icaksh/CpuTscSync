# CpuTscSync
[![Build Status](https://github.com/Seey6/CpuTscSync/workflows/CI/badge.svg?branch=master)](https://github.com/Seey6/CpuTscSync/actions)

It is a Lilu plugin, modify from CpuTscSync. It should solve some wake issues for AMD Mobile.

**WARNING**: Not all CPU need sync the TSC periodically(what voodootscsync and amdtscsync do). If you are not, add the bootarg `-cputsnoloop`

#### Boot-args
- `-cputsdbg` turns on debugging output
- `-cputsbeta` enables loading on unsupported osx
- `-cputsoff` disables kext loading
- `-cputsnoloop` disable the periodically sync

#### Credits
- [Apple](https://www.apple.com) for macOS  
- [vit9696](https://github.com/vit9696) for [Lilu.kext](https://github.com/vit9696/Lilu)
- [Voodoo Projects Team](http://forge.voodooprojects.org/p/voodootscsync/) for initial idea and implementation
- [RehabMan](https://github.com/RehabMan/VoodooTSCSync) for improved implementation
- [lvs1974](https://applelife.ru/members/lvs1974.53809/) for writing the software and maintaining it
- [Origin Project](https://github.com/acidanthera/CpuTscSync)
