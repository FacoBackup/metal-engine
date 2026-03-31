## **Engine Development & Code Style Guidelines**

### **1. Inclusion & Naming Conventions**

* **Absolute Paths:** Always use absolute paths relative to the project `src` or `include` root (e.g.,
  `#include <core/VulkanContext.h>`). This prevents header collisions and ensures LSP reliability.
* **Casing:** Use **camelCase** for all method names and variable names (e.g., `updateBuffer()`, `vertexCount`).

### **2. Architecture: Dependency Injection (DI)**

* **IContextMember:** Any class requiring external dependencies must extend `IContextMember`.
* **Dependency Definition:** Dependencies are defined via the `getDependencies()` method.
    * **Rule:** This method must be declared in the **header file**.
    * **Optimization:** Use **forward declarations** for all singleton types within the header to minimize
      include-chains.
* **Registration:** To register a new singleton for injection, you must modify `main.cpp`.

### **3. Specialized Engine Interfaces**

* **Serialization:** For any object requiring data persistence or state saving, extend the `Reflection` class.
* **Asynchronous Logic:** For logic that must loop throughout the engine's lifecycle (e.g., background streaming,
  physics stepping), implement the `IAsyncSync` interface.

### **4. Structural Integrity: Header vs. Source**

* **Implementation Files:** If a class contains logic, state, or non-inlined methods, it **must** have a corresponding
  `.cpp` file.
* **Pure Interfaces:** Use **Header-Only** files exclusively for interfaces consisting only of a virtual destructor and
  pure virtual methods.
* **Why:** This reduces "code bloat" in translation units and significantly speeds up compilation.

Here is the updated section with the dynamic project path and the CMake cache safety net added for you:

### **5. Tooling & Validation**

* **Suppress LSP Noise:** Ignore transient `clangd` or static analysis errors during active development. Trust the
  compiler's output as the source of truth over the IDE's "red squiggles."
* **Mandatory Compilation:** A task is not "finished" until the project compiles successfully.
* Dependencies injected via getDependencies method will never be null. Do dont null checks on them