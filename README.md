# BoardIEx
A [Lilu](https://github.com/acidanthera/Lilu) plugin that patches the `AppleGraphicsDisplayPolicy` (AGDP) kernel extension inside macOS to apply [Pike R. Alpha's `board-id` patch](https://pikeralpha.wordpress.com/2015/11/23/patching-applegraphicsdevicepolicy-kext). This kernel extension is created mainly for fixing black screen problems on AMD Navi cards (RX 5000 and RX 6000 series).

[WhateverGreen](https://github.com/acidanthera/WhateverGreen) has an equivalent device property field/boot argument of `agdpmod=pikera`, and [NootRX](https://github.com/ChefKissInc/NootRX) bundled the same fix by default. This lightweight kernel extension is made for people who want to use neither of those and still want the fix.
## How does this work?
macOS has a kernel extension named `AppleGraphicsDisplayPolicy` that dictates the display outputs of your machine using its `board-id`, for example `Mac-7BA5B2D9E42DDD94` that matches an iMac Pro. Inside it, Apple implemented a `ConfigMap` dictionary that maps different `board-id`s to specfic 'unusual' display output configurations (`Config1`, `Config2`, etc) that may not work on Hackintoshes. If your ID triggers one of these restrictive configurations, it may cause your GPU ports ceasing to function.

Why this is needed on AMD Navi cards you may ask?

Modern AMD GPU support in macOS mainly resides on three main drivers: `AMDRadeonX4000` for Polaris/Baffin cards, `AMDRadeonX5000` for Vega cards and `AMDRadeonX6000` for Navi cards. For some reason, the generic port configs for `AMDRadeonX4000` and `AMDRadeonX5000` and possibly other drivers too usually match all of the ports that regular non-Mac machines would have. The `AMDRadeonX6000` driver features a brand new driver stack, and it forces the GPU to route out signals to ports that exist on real Macs but don't exist on our non-Mac machines.

This Lilu plugin patches `board-id` to `board-ix`, nulling the check. When the check fails, `AppleGraphicsDisplayPolicy` fallbacks to the configuration `none`, which means - letting your GPU VBIOS dictate the video outputs.

## Credit
- Pike R. Alpha for [the patch](https://pikeralpha.wordpress.com/2015/11/23/patching-applegraphicsdevicepolicy-kext).