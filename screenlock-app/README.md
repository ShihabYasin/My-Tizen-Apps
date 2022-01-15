:warning: **This repository is not maintained anymore.**

# screenlock
A no nonsense ad-free screen locker for Tizen

Here's how the application will look after installation on Samsung Z1. **A tiny little lock icon**

![screenlock on Home Page](https://github.com/ShihabYasin/My-Tizen-Apps/blob/master/screenlock-app/home_screen.png)


Just click on ![screenlock](https://github.com/ShihabYasin/My-Tizen-Apps/blob/master/screenlock-app/icon.png) to lock your screen without pressing the hardware key.

### How to install the tpk
1. Download the <a href="Release/org.tizen.screenlock-1.0.0-arm.tpk">TPK</a> (Tizen Package)
2. sdb push org.tizen.screenlock-1.0.0-arm.tpk /tmp
3. sdb shell pkgcmd -q -i -t tpk -p /tmp/org.tizen.screenlock-1.0.0-arm.tpk
