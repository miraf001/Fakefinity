🖥️ Bezel-compensated DXGI screen mirroring tool for multi-monitor setups.

Fakefinity captures a selected monitor's display output and re-renders it as a seamless virtual ultrawide mirror across three monitors, skipping user-defined bezel gaps.

---

## ✨ Features

- Mirrors any monitor using DXGI output duplication
- Skips bezels for clean alignment between left, center, and right monitors
- INI-configurable monitor widths, bezels, window position, and height
- Hotkeys for quick control:
  - **Ctrl+Alt+Q** → Quit
  - **Ctrl+Alt+M** → Minimize
  - **Ctrl+Alt+R** → Restore & Focus

---

## ⚙️ INI Configuration (Fakefinity.ini)

```ini
WindowX=-997
WindowY=0
MonitorIndex=1
LeftWidth=1280
CenterWidth=1920
RightWidth=1280
BezelSize=50
VirtualHeight=1080
```

- `WindowX` and `WindowY` set the position of the mirror window.
- `MonitorIndex` is the zero-based index of the monitor to mirror.
- `LeftWidth`, `CenterWidth`, `RightWidth` define monitor widths in pixels.
- `BezelSize` is the pixel gap between displays (horizontal only).
- `VirtualHeight` can override the height if side monitors are taller.

---

## 🧰 Build Requirements

- Windows 10 or newer
- Visual Studio with C++ and Windows SDK
- Linked libraries:
  - d3d11.lib
  - dxgi.lib

---

## 🤖 Credits

Fakefinity was originally developed with assistance from OpenAI's ChatGPT, fine-tuned for C++ and DXGI.

- GitHub: [miraf001](https://github.com/miraf001)
- Tool concept and architecture by the user

---

## 📄 License

This project is licensed under the MIT License. See `LICENSE` for details.
