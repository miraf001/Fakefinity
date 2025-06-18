# Fakefinity

**Fakefinity** mirrors a virtual ultrawide display across 3 monitors with bezel compensation.  
It’s a workaround for broken or deprecated PLP support in AMD Eyefinity and NVIDIA Surround.

---

## 🧩 What It Does

- Mirrors a virtual monitor (e.g. from [`virtual-display-rs`](https://github.com/MolotovCherry/virtual-display-rs))
- Displays it in a borderless window stretched across 3 physical monitors
- Skips bezel gaps for seamless visual alignment
- Fully configurable via `Fakefinity.ini` (monitor widths, bezels, window position, height override)

---

## 🚀 How to Use

### 1. Create a Virtual Monitor
Use [`virtual-display-rs`](https://github.com/MolotovCherry/virtual-display-rs) to add a fake display.
Set its resolution as:
```
VirtualWidth = Left + Bezel + Center + Bezel + Right
VirtualHeight = Smallest height of your monitors
```

### 2. Edit `Fakefinity.ini`
```ini
WindowX=-1280
WindowY=0
MonitorIndex=3
LeftWidth=1280
CenterWidth=1920
RightWidth=1280
BezelSize=50
VirtualHeight=1080
```

**Explanation of keys:**
- `WindowX`, `WindowY`: Position of the mirrored window.  
  Typically, `WindowX = -LeftWidth` and `WindowY = 0` to center the window across 3 screens.
- `MonitorIndex`: The virtual monitor to mirror (see next section).
- `LeftWidth`, `CenterWidth`, `RightWidth`: Widths of each monitor.
- `BezelSize`: Horizontal gap between monitors. Set to `0` to disable bezel compensation.
- `VirtualHeight`: Optional override for output height (useful if side monitors are taller).

### 3. Run `Fakefinity.exe`

---

## 🔍 Finding the Correct `MonitorIndex`

This tells Fakefinity which display to mirror (typically your virtual ultrawide).

1. Open **Display Settings** → click **Identify**
2. Note the number shown on your virtual monitor (e.g. Display 4)
3. Then set:
   ```ini
   MonitorIndex = DisplayNumber - 1
   ```

> 💡 Example: If Windows shows **Display 4**, use `MonitorIndex = 3`

---

## ⌨️ Hotkeys

- `Ctrl+Alt+Q` → Quit
- `Ctrl+Alt+M` → Minimize
- `Ctrl+Alt+R` → Restore & focus

These are essential for interacting with Fakefinity in full-screen usage.

---

## 🖥️ Runtime Requirements

- Windows 10 or newer
- A virtual display (from [`virtual-display-rs`](https://github.com/MolotovCherry/virtual-display-rs))
- GPU with support for Desktop Duplication (most modern GPUs)

---

## 🛠 Build Requirements (for Developers)

- Visual Studio with C++ and Windows SDK
- Link against:
  - `d3d11.lib`
  - `dxgi.lib`

---

## 📄 License

MIT License — see the [LICENSE](LICENSE) file.
