
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

-- Break common tests into smaller, more focused test functions
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

    -- Transform component
    e:AddTransformComponent()
    ok("AddTransformComponent", e:HasTransformComponent())
    local t = e:GetTransformComponent()
    ok("Transform has translation field", t.Translation ~= nil)
    local oldX = t.Translation.x
    t.Translation.x = oldX + 2
    ok("Transform Set/Get changed translation", math.abs(t.Translation.x - (oldX + 2)) < 0.0001)

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

local function RunEntityRenderComponentTests()
    Log.Info("[Scene/Entity] render component tests")
    local scene = Engine.GetActiveScene()
    if not scene then
        Log.Info("No active scene available, skipping render tests")
        return
    end
    local e3 = scene:CreateEntity("RenderEntity", true)
    e3:AddRenderComponent()
    ok("AddRenderComponent", e3:HasRenderComponent())
    local renderComp = e3:GetRenderComponent()
    ok("RenderComponent IsVisible default true", renderComp:IsVisible())
    renderComp:SetVisible(false)
    ok("RenderComponent SetVisible works", not renderComp:IsVisible())
    e3:RemoveRenderComponent()
    ok("RemoveRenderComponent", not e3:HasRenderComponent())
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
    SafeCall(RunEntityRenderComponentTests, "EntityRenderComponentTests")
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
    Input.AddEventListener(InputEventType.KeyPressed, function(ev)
        okAdded = true
    end)
    ok("Input.AddEventListener (no crash on registration)", true)
end

local function RunAllTests()
--[[
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
 ]]
    SafeCall(RunScriptingLoadTests, "ScriptingLoadTests")
--[[
    SafeCall(RunLuaScriptInstanceTest, "LuaScriptInstanceTest")
    SafeCall(RunInputTests, "InputTests")
 ]]
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