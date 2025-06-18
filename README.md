# Fakefinity

**Fakefinity** mirrors a virtual ultrawide display across 3 monitors with bezel compensation.
It’s a workaround for broken or deprecated PLP support in AMD Eyefinity and NVIDIA Surround.

---

## What It Does

* Mirrors a virtual monitor (e.g. from [`virtual-display-rs`](https://github.com/MolotovCherry/virtual-display-rs))
* Displays it in a borderless window stretched across 3 physical screens
* Skips bezel gaps for seamless visual alignment
* Configurable widths, bezels, and position via `Fakefinity.ini`

---

## How to Use

1. **Create a virtual monitor** using [`virtual-display-rs`](https://github.com/MolotovCherry/virtual-display-rs)

   * Set its resolution to:
     `Left + Bezel + Center + Bezel + Right` × `Height`

2. **Edit `Fakefinity.ini`** to match your layout:

   ```ini
   WindowX=-997
   WindowY=0
   MonitorIndex=3
   LeftWidth=1280
   CenterWidth=1920
   RightWidth=1280
   BezelSize=50
   VirtualHeight=1080
   ```
WindowX and Y - position of the virtual screen on your triple monitor setup, usualy X will be -width of your side monitor and Y 0


MonitorIndex - see below

LeftWidth, CenterWidth, RightWidth - Width of each of your monitors

BezelSize - size of the bezel, you can use 0 to disable bezel functionality.


3. **Run `Fakefinity.exe`**

---

## Finding the Correct `MonitorIndex`

This tells Fakefinity which display to capture (your virtual monitor).

* Open **Display Settings** and click **Identify**
* Find the number shown on the virtual screen (usually the largest one)
* Then set:

  ```ini
  MonitorIndex = ThatNumber - 1
  ```

  *(Windows starts at 1, Fakefinity uses 0-based indexing)*

**Example:**
If Windows labels your virtual monitor as **Display 4**, use:

```ini
MonitorIndex = 3
```

---

## Hotkeys

* `Ctrl+Alt+Q` → Quit
* `Ctrl+Alt+M` → Minimize
* `Ctrl+Alt+R` → Restore & focus

---

## Runtime Requirements

* Windows 10 or newer
* A virtual display created by [`virtual-display-rs`](https://github.com/MolotovCherry/virtual-display-rs)
* A GPU that supports desktop duplication (most modern GPUs do)

---

## Build Requirements (for developers)

* Visual Studio with C++ and Windows SDK
* Linked libraries:

  * `d3d11.lib`
  * `dxgi.lib`

---

## License

MIT License — see the `LICENSE` file.
