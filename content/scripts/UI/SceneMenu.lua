function OnDrawUI()
	ImGui.Begin("Main menu")
	if ImGui.Button("Back to menu", 300, 20) then 
		Log.Info("Opening ui test scene")
		AssetManager.OpenScene("./UITest.scene")
	end
	ImGui.End()
end