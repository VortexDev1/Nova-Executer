# Nova Executer

Nova Executer is a custom Lua executer designed for **Blockman Go v2.118.5**, built to provide developers and modders with enhanced scripting and debugging capabilities.  

## ⚙️ Features
- Compatible with **Blockman Go 2.118.5**
- Executes Lua scripts in real time
- Simple and lightweight user interface
- Supports both internal and external script loading
- Designed for fast performance and stability
- DONST SUPPORT INJECTION (RIGHT NOW)


## 🧰 Requirements
- Android 5.0 (Lollipop) or higher  
- Android NDK / SDK if building natively  
- AIDE or Android Studio for development  

## 🛠️ Building
If using **AIDE**:
1. Copy the project to `/sdcard/AppProjects/NovaExecuter/`
2. Open the project in AIDE
3. Tap the **Run** button to build and install

If using **Android Studio**:
```bash
git clone https://github.com/YourUsername/NovaExecuter.git
cd NovaExecuter
gradlew assembleDebug
