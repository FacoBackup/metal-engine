### Metal Engine Lua Scripting Skill

The Metal Engine uses a module-based Lua scripting system powered by `sol2`. 
Each script is isolated in its own environment and follows a unified component lifecycle.

#### Script Structure
Every script file must return a table (acting as a class prototype) with the following optional methods:
- `onCreate()`: Called when the script component is first initialized or when the "Play" event is triggered.
- `onUpdate()`: Called every frame during the engine's sync phase (main thread) or async phase (worker thread), depending on the component's `runAsync` flag.
- `onDestroy()`: Called when the component is removed or when the "Stop" event is triggered.

Example:
```lua
local MyScript = {}

function MyScript:onCreate()
    self.speed = 5.0
    print("Script created for entity: " .. tostring(self.this_entity))
end

function MyScript:onUpdate()
    local transform = world:getTransform(self.this_entity)
    transform.position.x = transform.position.x + self.speed * deltaTime
end

function MyScript:onDestroy()
    print("Script destroyed")
end

return MyScript
```

#### Global Bindings
- `world`: The `WorldRepository` instance.
- `deltaTime`: The time elapsed since the last frame (seconds).
- `ComponentType`: Enum for component IDs (e.g., `ComponentType.TRANSFORM`, `ComponentType.SCRIPT`).

#### WorldRepository API
- `world:createEntity()`: Creates and returns a new `entt::entity`.
- `world:deleteEntity(entity)`: Removes an entity and all its components.
- `world:hasComponent(entity, type)`: Returns true if the entity has the specified component type.
- `world:getTransform(entity)`: Returns the `TransformComponent` for the entity.
- `world:getMetadata(entity)`: Returns the `MetadataComponent` (name, tags).
- `world:getScript(entity)`: Returns the `ScriptComponent`.

#### Math Library (GLM)
- `vec3(x, y, z)`, `quat(w, x, y, z)`, `mat4(1.0)`
- Basic arithmetic operators (`+`, `-`, `*`, `/`) are overloaded for vectors.

#### AI Assistant Capabilities (MCP Tools)
You can manipulate the world and inspect the engine state using the following tools:

**World Management (`WorldToolProvider`):**
- `create_entity()`: Create a new entity.
- `delete_entity(id)`: Remove an entity by ID.
- `add_component(entity_id, component_type_id)`: Add a component to an entity.
- `list_entities()`: Get a list of all entities in the scene.
- `get_entity_info(id)`: Get detailed info (name, components) about an entity.
- `list_component_types()`: List all available components and their IDs.

**Inspection & Modification (`InspectionToolProvider`):**
- `listRepositories()`: List all global engine services (repositories) and their fields.
- `getComponentInfo(entity_id, component_type_id)`: Inspect the current field values of a component.
- `changeComponentField(entity_id, component_type_id, field_path, value)`: Update a component's property (e.g., set `position.y` to `10`).
- `changeRepositoryField(repository_class_name, field_path, value)`: Update a global engine setting.

#### Best Practices
1. **Isolation**: Scripts should use `self` to store instance-specific data.
2. **Performance**: Use `runAsync` in `ScriptComponent` for logic that doesn't need to be on the main thread.
3. **Safety**: The assistant should always check `list_component_types` before adding components to ensure the correct ID is used.
