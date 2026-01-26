DrawUI = false

function OnDrawUI()
	if not DrawUI then return end
	ImGui.Begin("Main menu")
	if ImGui.Button("Start", 200, 20) then 
		Log.Info("Opening main scene")
		AssetManager.OpenScene("./Scene.scene")
	end
	if ImGui.Button("NavMesh", 200, 20) then 
		Log.Info("Opening navmesh scene")
		AssetManager.OpenScene("./Navmesh.scene")
	end
	ImGui.End()
end

inputHandle = nil
function OnStart()
	inputHandle = Input.AddEventListener(InputEventType.KeyPressed,
		function (event)
			if event.key == Key.Escape then 
				DrawUI = not DrawUI
			end
		end
	)
end

function OnDestroy()
	if inputHandle ~= nil then
		Input.RemoveEventListener(InputEventType.KeyPressed, inputHandle)
	end
end