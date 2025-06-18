Fakefinity
Fakefinity mirrors a virtual ultrawide display across 3 monitors with bezel compensation.
It’s a workaround for broken or deprecated PLP support in AMD Eyefinity and NVIDIA Surround.

🖼️ What It Does
Mirrors a virtual monitor (e.g. from virtual-display-rs)

Displays it in a borderless window stretched across 3 physical screens

Skips bezel gaps for seamless visual alignment

Configurable widths, bezels, and position via Fakefinity.ini

⚙️ How to Use
Create a virtual monitor using virtual-display-rs

Set its resolution to:
Left + Bezel + Center + Bezel + Right × Height

Edit Fakefinity.ini to match your layout:

ini
Copy
Edit
WindowX=-997
WindowY=0
MonitorIndex=3
LeftWidth=1280
CenterWidth=1920
RightWidth=1280
BezelSize=50
VirtualHeight=1080
Run Fakefinity.exe

🧭 Finding the Correct MonitorIndex
This tells Fakefinity which display to capture (your virtual monitor).

Open Display Settings → click Identify

Find the number shown on the virtual screen (usually the largest one)

Set:
MonitorIndex = ThatNumber - 1
(Windows numbers from 1, Fakefinity uses 0-based index)

Example:
If Windows labels your virtual monitor as Display 4, use:

ini
Copy
Edit
MonitorIndex = 3
⌨️ Hotkeys
Ctrl+Alt+Q → Quit

Ctrl+Alt+M → Minimize

Ctrl+Alt+R → Restore & focus

▶️ Runtime Requirements
Windows 10 or newer

A virtual display created by virtual-display-rs

A GPU that supports desktop duplication (most modern GPUs do)

🛠️ Build Requirements (if compiling yourself)
Visual Studio with C++ and Windows SDK

Libraries:

d3d11.lib

dxgi.lib

📝 License
MIT — see LICENSE file.
