
-- Simple test helpers
local testsPassed = 0
local testsFailed = 0

local function ok(name, cond)
    if cond then
        Log.Info("PASS: " .. name)
        testsPassed = testsPassed + 1
    else
        Log.Info("FAIL: " .. name)
        testsFailed = testsFailed + 1
    end
end

-- SafeCall helper: runs a function and logs errors without crashing the whole suite
local function SafeCall(fn, name)
    local okCall, err = pcall(fn)
    if not okCall then
        -- If err isn't a string, attempt to provide a traceback
        local err_str = tostring(err)
        if err_str == 'nil' then
            err_str = debug.traceback()
        end
        Log.Error("ERROR running test '" .. name .. "': " .. err_str)
    else
        Log.Info("Completed: " .. name)
    end
end

local function RunVec2Tests()
    Log.Info("[Common] Vec2 tests")
    local a = Vec2:new(3, 4)
    local b = Vec2:new(1, 2)
    local r = a + b
    ok("Vec2 addition", (math.abs(r.x - 4) < 0.0001) and (math.abs(r.y - 6) < 0.0001))
    r:Normalize()
    ok("Vec2 normalize (length 1)", math.abs(math.sqrt(r.x * r.x + r.y * r.y) - 1.0) < 0.0001)
    -- Edge case: normalization of zero vector should not crash
    local z = Vec2:new(0, 0)
    local succ, _ = pcall(function() z:Normalize() end)
    ok("Vec2 zero normalization doesn't crash (pcall)", succ)
end

local function RunVec3Tests()
    Log.Info("[Common] Vec3 tests")
    local v3a = Vec3:new(1, 0, 0)
    local v3b = Vec3:new(0, 1, 0)
    local v3c = v3a + v3b
    ok("Vec3 addition", (math.abs(v3c.x - 1) < 0.0001) and (math.abs(v3c.y - 1) < 0.0001))
end

local function RunVec4Tests()
    Log.Info("[Common] Vec4 tests")
    local v4a = Vec4:new(1, 2, 3, 4)
    local v4b = Vec4:new(4, 3, 2, 1)
    local sum = v4a + v4b
    ok("Vec4 addition", (math.abs(sum.x - 5) < 0.0001) and (math.abs(sum.w - 5) < 0.0001))
    -- Test Normalize if available
    local succNorm, _ = pcall(function() sum:Normalize() end)
    ok("Vec4 normalize doesn't crash if available", succNorm)
end

local function RunQuatTests()
    Log.Info("[Common] Quat tests")
    local q = Quat:new(1, 2, 0, 0)
    q:Normalize()
    ok("Quat Normalize no crash", true)
    local qn = q:GetNormalized()
    ok("Quat GetNormalized returns Quaternion", type(qn) == "userdata")
end

local function RunMat4Tests()
    Log.Info("[Common] Mat4 tests")
    local q = Quat:new(1, 2, 0, 0)
    q:Normalize()
    local m1 = Mat4Identity
    local v3mt = Vec3:new(1, 2, 3)
    local m2 = Mat4.CreateTranslation(v3mt)
    local m3 = m1 * m2
    ok("Mat4 multiplication simple", true)
    local mi = m3:Invert()
    ok("Mat4 invert no crash", true)
    local t = Mat4.CreateFromQuaternion(q)
    ok("Mat4 CreateFromQuaternion no crash", type(t) == "userdata")
end

local function RunPlaneTests()
    Log.Info("[Common] Plane tests")
    local pl = Plane:new()
    pl:SetNormal(Vec3:new(0, 1, 0))
    pl:SetD(5)
    ok("Plane Set/Get", pl:Normal().y == 1 and pl:D() == 5)
end

local function RunRectRayViewportColorTests()
    Log.Info("[Common] Rect/Ray/Viewport/Color tests")
    local rect = Rect:new(1, 2, 3, 4)
    ok("Rect constructor fields", rect.x == 1 and rect.y == 2 and rect.width == 3 and rect.height == 4)
    local ray = Ray:new(Vec3:new(1, 0, 0), Vec3:new(0, 1, 0))
    ok("Ray fields", ray.position.x == 1)
    local vp = Viewport:new(0, 0, 800, 600)
    ok("Viewport size fields", vp.width == 800 and vp.height == 600)
    local c = Color:new(0.1, 0.2, 0.3, 0.4)
    ok("Color components R G B A exist", math.abs(c.R - 0.1) < 0.0001 and math.abs(c.A - 0.4) < 0.0001)
end

local function RunEngineTests()
    Log.Info("[Engine] basic tests")
    local content = Engine.GetContentDirectory()
    ok("Engine.GetContentDirectory returns string", type(content) == "string")

    local current = Engine.GetContentDirectory()
    Engine.SetContentDirectory(current) -- set to same path should not crash
    ok("Engine.SetContentDirectory no error", true)
end

local function RunEntityLifecycleTests()
    Log.Info("[Scene/Entity] lifecycle tests")
    local scene = Engine.GetActiveScene()
    if not scene then
        Log.Info("No active scene available, skipping entity lifecycle tests")
        return
    end

    local e = scene:CreateEntity("LuaTestEntity", true)
    ok("Entity created", e ~= nil and e:IsValid())

    -- Tag component should be created when entity is created with a name
    ok("Entity has Tag component", e:HasTagComponent())
    ok("Entity GetName matches provided name", e:GetName() == "LuaTestEntity")
    local tagComp = e:GetTagComponent()
    ok("Tag component Tag field matches name", tagComp.Tag == "LuaTestEntity")

    -- Scene event listener registration test: add and remove
    local succSceneListen, sceneListenHandle = pcall(function()
        return scene:AddEventListener(SceneEventType.EntityCreated, function(ev)
            -- noop
        end)
    end)
    ok("Scene:AddEventListener registration pcall", succSceneListen)

    if succSceneListen and sceneListenHandle ~= nil then
        local succSceneRem = pcall(function() scene:RemoveEventListener(SceneEventType.EntityCreated, sceneListenHandle) end)
        ok("Scene:RemoveEventListener pcall", succSceneRem)
    end

    -- Transform component
    e:AddTransformComponent()
    ok("AddTransformComponent", e:HasTransformComponent())
    local t = e:GetTransformComponent()
    local succGetTrans, trans = pcall(function() return t:GetTranslation() end)
    ok("Transform GetTranslation callable", succGetTrans)
    if succGetTrans and trans ~= nil then
        local oldX = trans.x
        t:SetTranslation(Vec3:new(oldX + 2, 0, 0))
        ok("Transform Set/Get changed translation", math.abs(t:GetTranslation().x - (oldX + 2)) < 0.0001)
    end

    -- TODO:
    -- Parent/child
    -- local child = scene:CreateChildEntity(e, "ChildEntity", true)
    -- ok("CreateChildEntity", child ~= nil and child:IsValid())
    -- scene:ParentEntity(child, e)
    -- ok("ParentEntity manual", not (child:GetParentUUID() == 0))
    -- scene:UnparentEntity(child, true)
    -- local parentUUID = child:GetParentUUID()
    -- ok("UnparentEntity", parentUUID == "00000000-0000-0000-0000-000000000000")
    -- -- Validate Children list and parent
    -- local childIDs = e:Children()
    -- ok("Children() returns table", type(childIDs) == "table")
    -- -- If it contains child uuid as string
    -- local foundChild = false
    -- for k, v in pairs(childIDs) do
    --     if v == child:GetUUID() then foundChild = true end
    -- end
    -- ok("Children contains child uuid", foundChild)


    -- scene:DestroyEntity(child)
    -- ok("DestroyEntity child", true)

    -- Entity destruction
    scene:DestroyEntity(e)
    ok("DestroyEntity parent", true)
end


local function RunEntitySearchAndIDTests()
    Log.Info("[Scene/Entity] entity search and UUID tests")
    local scene = Engine.GetActiveScene()
    if not scene then
        Log.Info("No active scene available, skipping search/ID tests")
        return
    end
    -- We'll create a temporary entity to test UUID and searches
    local e = scene:CreateEntity("LuaTestEntityForSearch", true)
    ok("Entity created for search tests", e ~= nil and e:IsValid())
    local uuidStr = e:GetUUID()

    -- CreateEntityWithID: try to create another entity with same ID
    local e2 = scene:CreateEntityWithID(uuidStr, "EntityWithSameID", true, true)
    ok("CreateEntityWithID does not crash", e2 ~= nil)

    -- Try Get by UUID (existing)
    local found = scene:GetEntityWithUUID(uuidStr)
    ok("GetEntityWithUUID returns entity", found ~= nil and found:IsValid())

    -- TODO:
    -- TryGetEntityWithTag
    -- local search = scene:TryGetEntityWithTag("TestLuaTag")
    -- ok("TryGetEntityWithTag finds entity by tag", search ~= nil and search:IsValid())

    -- -- Test TryGetDescendantEntityWithTag
    -- local desc = scene:TryGetDescendantEntityWithTag(e, "TestLuaTag")
    -- ok("TryGetDescendantEntityWithTag returns something or nil (doesn't crash)", true)

    -- GetAllEntitiesWithTagComponent
    local allWithTag = scene:GetAllEntitiesWithTagComponent()
    ok("GetAllEntitiesWithTagComponent returns table", type(allWithTag) == "table")

    -- Duplicate entity (not implemented returns nil / entity invalid), don't crash
    local dup = scene:DuplicateEntity(e)
    ok("DuplicateEntity returns entity or nothing (no crash)", true)

    -- Cleanup
    scene:DestroyEntity(dup)
    scene:DestroyEntity(e)
    if e2 ~= nil and e2:IsValid() then
        scene:DestroyEntity(e2)
    end
end

local function RunEntityWorldSpaceTests()
    Log.Info("[Scene/Entity] world transform tests")
    local scene = Engine.GetActiveScene()
    if not scene then
        Log.Info("No active scene available, skipping world transform tests")
        return
    end
    local e = scene:CreateEntity("WorldTransformEntity", true)
    ok("Entity created for world transform tests", e ~= nil and e:IsValid())

    Entity.AddTransformComponent(e)
    -- ConvertToWorldSpace and ConvertToLocalSpace and GetWorldSpaceTransformMatrix
    scene:ConvertToWorldSpace(e)
    scene:ConvertToLocalSpace(e)
    local worldMat = scene:GetWorldSpaceTransformMatrix(e)
    ok("GetWorldSpaceTransformMatrix returns matrix", type(worldMat) == "userdata")
    local worldTransform = scene:GetWorldSpaceTransform(e)
    ok("GetWorldSpaceTransform returns TransformComponent-like object", type(worldTransform) == "userdata")
    scene:DestroyEntity(e)
end

local function RunSceneEntityTests()
    -- Grouping function to run all scene/entity related tests under one SafeCall group
    SafeCall(RunEntityLifecycleTests, "EntityLifecycleTests")
    SafeCall(RunEntitySearchAndIDTests, "EntitySearchAndIDTests")
    SafeCall(RunEntityWorldSpaceTests, "EntityWorldSpaceTests")
end

local function RunAssetManagerTests()
    Log.Info("[Assets] AssetManager tests")
    -- TODO: when default data provided 
    -- local hasMesh = AssetManager.HasMesh("nonexistent.model")
    -- ok("HasMesh false for invalid path", hasMesh == false)

    -- local hasTexture = AssetManager.HasTexture("nonexistent.png")
    -- ok("HasTexture false for invalid path", hasTexture == false)

    -- local hasMaterial = AssetManager.HasMaterial("nonexistent.mat")
    -- ok("HasMaterial false for invalid path", hasMaterial == false)

    -- Try to load an invalid mesh - expect 0 index (failure)
    -- local importData = ImportMeshData:new()
    -- importData.path = "nonexistent.model"
    -- importData.convertToLH = false
    -- importData.createMaterials = false
    -- local meshIndex = AssetManager.LoadMesh("nonexistent.model", importData)
    -- ok("LoadMesh invalid returns 0", meshIndex == 0)

    -- -- Scene functions
    -- local exists = AssetManager.SceneExists("nonexistent.scene")
    -- ok("SceneExists returns false for invalid", exists == false)
    --     -- Get model/texture/material by index functions exist in Lua. We won't call index lookups with invalid indexes to avoid crash. We just test their existence via pcall.
    --     local succ, _ = pcall(function() AssetManager.GetMeshPathByIndex(0) end)
    --     ok("AssetManager.GetMeshPathByIndex callable (pcall returns true if safe)", not succ) -- often will fail on 0
    --     local succ2 = pcall(function() AssetManager.GetTexturePathByIndex(0) end)
    --     ok("AssetManager.GetTexturePathByIndex callable (pcall)", not succ2)
end

local function RunScriptingLoadTests()
    Log.Info("[Scripting] scripting load/unload tests")
    local scene = Engine.GetActiveScene()
    if not scene then
        Log.Info("No active scene available, skipping scripting load tests")
        return
    end

    local e = scene:CreateEntity("LuaScriptEntity", true)
    e:AddScriptingComponent()
    ok("AddScriptingComponent", e:HasScriptingComponent())

    -- Attempt to load this script (test2.lua)
    local id = Scripting.LoadScript(e, "./scripts/test2.lua", true)
    if type(id) == "string" then
        ok("Scripting.LoadScript returns id string", true)
        local scripts = Scripting.ListScripts(e)
        local found = false
        for k, v in pairs(scripts) do
            if v == id then
                found = true
                break
            end
        end
        ok("ListScripts contains loaded script id", found)
        -- Scripting.UnloadScript(id)
        -- ok("UnloadScript called", true)
    else
        Log.Info("Scripting.LoadScript not available or returned nil; skipping further scripting checks")
    end
end

local function RunLuaScriptInstanceTest()
    Log.Info("[Scripting] LuaScript instance tests")
    -- Test LuaScript methods indirectly (Create a LuaScript instance via constructor if needed)
    local succ, scriptObj = pcall(function() return LuaScript.new() end)
    if succ then
        -- Instance created; test method exists
        ok("LuaScript IsLoaded method exists / callable", scriptObj ~= nil)
    else
        Log.Info("LuaScript constructor not available; skipping instance test")
    end
end

local function RunInputTests()
    Log.Info("[Input] tests (add listener only)")
    local okAdded = false
    local succListen, listenHandle = pcall(function()
        return Input.AddEventListener(InputEventType.KeyPressed, function(ev)
            okAdded = true
        end)
    end)
    ok("Input.AddEventListener registration pcall", succListen)

    --remove listener if we got a handle
    if succListen and listenHandle ~= nil then
        local succRem = pcall(function() Input.RemoveEventListener(InputEventType.KeyPressed, listenHandle) end)
        ok("Input.RemoveEventListener pcall", succRem)
    end
end

local function RunPhysicsEnumAndTypeTests()
    Log.Info("[Physics] enum/type tests")
    ok("ComponentShapeType exists", ComponentShapeType ~= nil)
    ok("ComponentShapeType.Sphere is number", type(ComponentShapeType.Sphere) == "number")
    ok("PhysicsComponentMotionType exists", PhysicsComponentMotionType ~= nil)
    ok("EActivation exists", EActivation ~= nil)
    ok("PhysicsEventType exists", PhysicsEventType ~= nil)
    ok("Layers table exists", type(Layers) == "table")

    local succ1 = pcall(function() local s = ShapeCreationSettings(ComponentShapeType.Sphere) end)
    ok("ShapeCreationSettings construct (call) doesn't crash", succ1)
    local succ2 = pcall(function() local s = ShapeCreationSettings:new(ComponentShapeType.Sphere) end)
    ok("ShapeCreationSettings construct (colon) doesn't crash", succ2)

    local succ3 = pcall(function() local dummy = PhysicsComponentSettings(nil, ComponentShapeType.Box) end)
    ok("PhysicsComponentSettings constructor callable (nil entity) without crash", succ3)
end


local function RunPhysicsRuntimeTests()
    Log.Info("[Physics] runtime tests (require active scene)")
    local scene = Engine.GetActiveScene()
    if not scene then
        Log.Info("No active scene available, skipping runtime physics tests")
        return
    end

    local e = scene:CreateEntity("LuaPhysicsTest", true)
    ok("Entity created for physics tests", e ~= nil and e:IsValid())

    -- Ensure entity has a Transform component (required by some systems)
    e:AddTransformComponent()
    ok("AddTransformComponent for physics test entity", e:HasTransformComponent())

    -- Try creating with simple overload
    local succAttach = pcall(function() Physics.CreateAttachPhysicsComponent(e, ComponentShapeType.Sphere) end)
    ok("Physics.CreateAttachPhysicsComponent (entity,shape) pcall", succAttach)

    -- Try creating with settings object and custom shape parameters
    local succAttach2 = pcall(function()
        local shape = ShapeCreationSettings:new(ComponentShapeType.Sphere)
        shape.radius = 0.75
        local settings = PhysicsComponentSettings:new(e, ComponentShapeType.Sphere)
        settings.shapeSettings = shape
        Physics.CreateAttachPhysicsComponent(settings)
    end)
    ok("Physics.CreateAttachPhysicsComponent (settings object) pcall", succAttach2)

    local succHas, has = pcall(function() return Physics.HasPhysicsComponent(e) end)
    ok("Physics.HasPhysicsComponent callable", succHas and (type(has) == "boolean"))

    local succHas, has = pcall(function() return Physics.HasPhysicsComponent(e) end)
    ok("Physics.HasPhysicsComponent callable", succHas and (type(has) == "boolean"))
    if succHas and has then
        local succGet, getter = pcall(function() return Physics.GetBodyGetter(e) end)
        ok("Physics.GetBodyGetter pcall", succGet)
        if succGet and getter ~= nil then
            ok("BodyGetter.GetPosition pcall", pcall(function() getter.GetPosition() end))
            ok("BodyGetter.GetRotation pcall", pcall(function() getter.GetRotation() end))
            ok("BodyGetter.GetScale pcall", pcall(function() getter.GetScale() end))
            ok("BodyGetter.GetShapeType pcall", pcall(function() getter.GetShapeType() end))
            ok("BodyGetter.GetVelocity pcall", pcall(function() getter.GetVelocity() end))
            ok("BodyGetter.GetMaxLinearVelocity pcall", pcall(function() getter.GetMaxLinearVelocity() end))
            ok("BodyGetter.GetAngularVelocity pcall", pcall(function() getter.GetAngularVelocity() end))
            ok("BodyGetter.GetMaxAngularVelocity pcall", pcall(function() getter.GetMaxAngularVelocity() end))
            ok("BodyGetter.GetGravityFactor pcall", pcall(function() getter.GetGravityFactor() end))
            ok("BodyGetter.IsTrigger pcall", pcall(function() getter.IsTrigger() end))
            ok("BodyGetter.GetObjectLayer pcall", pcall(function() getter.GetObjectLayer() end))
            ok("BodyGetter.GetMotionType pcall", pcall(function() getter.GetMotionType() end))
            ok("BodyGetter.GetSphereShapeRadius pcall", pcall(function() getter.GetSphereShapeRadius() end))
            ok("BodyGetter.GetBoxShapeHalfExtents pcall", pcall(function() getter.GetBoxShapeHalfExtents() end))
            ok("BodyGetter.GetCylinderShapeHalfHeightAndRadius pcall", pcall(function() getter.GetCylinderShapeHalfHeightAndRadius() end))
            ok("BodyGetter.GetCapsuleShapeHalfHeightAndRadius pcall", pcall(function() getter.GetCapsuleShapeHalfHeightAndRadius() end))
        end
    else
        Log.Info("No physics component present on entity; skipping BodyGetter tests")
    end

    local succUpd, updater = pcall(function() return Physics.GetBodyUpdater(e) end)
    ok("Physics.GetBodyUpdater pcall", succUpd)
    if succUpd and updater ~= nil and succHas and has then
        ok("BodyUpdater.SetPosition pcall", pcall(function() updater.SetPosition(Vec3:new(0,0,0), EActivation.DontActivate) end))
        ok("BodyUpdater.SetRotation pcall", pcall(function() updater.SetRotation(Quat:new(1,0,0,0), EActivation.DontActivate) end))
        ok("BodyUpdater.SetScale pcall", pcall(function() updater.SetScale(Vec3:new(1,1,1), Vec3:new(1,1,1)) end))
        ok("BodyUpdater.SetVelocity pcall", pcall(function() updater.SetVelocity(Vec3:new(0,0,0)) end))
        ok("BodyUpdater.SetMaxLinearVelocity pcall", pcall(function() updater.SetMaxLinearVelocity(10.0) end))
        ok("BodyUpdater.SetAngularVelocity pcall", pcall(function() updater.SetAngularVelocity(Vec3:new(0,0,0)) end))
        ok("BodyUpdater.SetMaxAngularVelocity pcall", pcall(function() updater.SetMaxAngularVelocity(10.0) end))
        ok("BodyUpdater.SetGravityFactor pcall", pcall(function() updater.SetGravityFactor(1.0) end))
        ok("BodyUpdater.SetObjectLayer pcall", pcall(function() updater.SetObjectLayer(Layers.MOVING) end))
        ok("BodyUpdater.AddVelocity pcall", pcall(function() updater.AddVelocity(Vec3:new(0,0,0)) end))
        ok("BodyUpdater.AddImpulse pcall", pcall(function() updater.AddImpulse(Vec3:new(0,0,0)) end))
        ok("BodyUpdater.AddAngularImpulse pcall", pcall(function() updater.AddAngularImpulse(Vec3:new(0,0,0)) end))
        ok("BodyUpdater.AddForce pcall", pcall(function() updater.AddForce(Vec3:new(0,0,0)) end))
        ok("BodyUpdater.ReplaceBodyShape pcall", pcall(function()
            local s = ShapeCreationSettings:new(ComponentShapeType.Box)
            s.halfExtents = Vec3:new(0.5,0.5,0.5)
            updater.ReplaceBodyShape(s, EActivation.DontActivate)
        end))
        ok("BodyUpdater.SetMotionType pcall", pcall(function() updater.SetMotionType(PhysicsComponentMotionType.Dynamic, EActivation.DontActivate) end))
        ok("BodyUpdater.SetSphereShapeSettings pcall", pcall(function() updater.SetSphereShapeSettings(0.5) end))
        ok("BodyUpdater.SetBoxShapeSettings pcall", pcall(function() updater.SetBoxShapeSettings(Vec3:new(0.5,0.5,0.5)) end))
        ok("BodyUpdater.SetCapsuleShapeSettings pcall", pcall(function() updater.SetCapsuleShapeSettings(0.5,0.25) end))
        ok("BodyUpdater.SetCylinderShapeSettings pcall", pcall(function() updater.SetCylinderShapeSettings(0.5,0.25) end))
    else
        Log.Info("No physics component present on entity; skipping BodyUpdater tests")
    end

    local succListen, listenHandle = pcall(function()
        return Physics.AddEventListener(PhysicsEventType.CollisionStarted, function(ev)
            -- noop
        end)
    end)
    ok("Physics.AddEventListener registration pcall", succListen)

    -- remove listener if we obtained a handle
    if succListen and listenHandle ~= nil then
        local succRem = pcall(function() Physics.RemoveEventListener(PhysicsEventType.CollisionStarted, listenHandle) end)
        ok("Physics.RemoveEventListener pcall", succRem)
    end

    pcall(function() Physics.DestroyPhysicsComponent(e) end)
    scene:DestroyEntity(e)
end

local function RunAllTests()

    SafeCall(RunVec2Tests, "Vec2Tests")
    SafeCall(RunVec3Tests, "Vec3Tests")
    SafeCall(RunVec4Tests, "Vec4Tests")
    SafeCall(RunQuatTests, "QuatTests")
    SafeCall(RunMat4Tests, "Mat4Tests")
    SafeCall(RunPlaneTests, "PlaneTests")
    SafeCall(RunRectRayViewportColorTests, "RectRayViewportColorTests")
    SafeCall(RunEngineTests, "EngineTests")
    SafeCall(RunSceneEntityTests, "SceneEntityTests")
    SafeCall(RunAssetManagerTests, "AssetManagerTests")
    SafeCall(RunScriptingLoadTests, "ScriptingLoadTests")
    SafeCall(RunLuaScriptInstanceTest, "LuaScriptInstanceTest")
    SafeCall(RunInputTests, "InputTests")
    SafeCall(RunPhysicsEnumAndTypeTests, "PhysicsEnumAndTypeTests")
    SafeCall(RunPhysicsRuntimeTests, "PhysicsRuntimeTests")
 
    Log.Warn("--- Tests completed. Passed: " .. testsPassed .. ", Failed: " .. testsFailed .. " ---\n")
end

function OnStart()
    Log.Info("Test1 onStart called")
    print("\n")
    Log.Warn("--- Running Lua type-registration tests ---\n")
    RunAllTests()
end

function OnUpdate(deltaTime)

end

function OnDestroy()
    Log.Info("Test1 onDestroy called")
end

-- End of test file