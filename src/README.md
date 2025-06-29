## Project structure

### Common
Common structures. Including enums, abstract classes, interfaces and util classes

### UI
`GuiContext` handles ImGui life-cycle
`GLFWContext` handles glfw window life-cycle

Services, repositories and other structures are all related to the ImGui rendering.

### Runtime
`EngineContext` handles the rendering engine and world life-cycle

Game runtime stuff, like inputs, world, camera and engine runtime.

### Renderer

`VulkanContext` handles the graphics API life-cycle, in this case, Vulkan.

Services, repositories and other structures are all related to the Vulkan pipeline and rendering basic structures.


### Context classes

Are basically façades that will create an interface for external communication with the given SINGLETONS. They are singletons
instantiated at the engine startup.

These classes are also responsible for instantiating services and repositories related to their SINGLETONS.

The `ApplicationContext` singleton can be obtained via the `Metal::ContextProvider::Get` method.