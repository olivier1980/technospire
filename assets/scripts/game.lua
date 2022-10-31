onEnter = function(scene)
    if scene.isVisited then
        if scene.entranceDialog then
            addDialogs(scene.entranceDialog)
        end
    else
        scene.isVisited = true
        if scene.firstDialog then
            addDialogs(scene.firstDialog)
        end
    end

end

onExit = function(scene)

end

gotoDeath = function()
    clearInventory()
    moveToDestination("SceneDeath")
end


function findDialog(actionItem, action)
    local defaultDialog = {
        look = "<missing look dialogue>",
        open = "It won't open!",
        use = "You can't use that.",
        take = "You can't take it!",
        magicLight = "Lightning won't work here.",
        magicShield = "That can't be used as a shield.",
        magicOrb = "This spell won't work here.",
    }

    if actionItem["dialogs"] ~= nil and actionItem.dialogs[action] then
        return actionItem.dialogs[action]
    else
        for k,v in pairs(defaultDialog) do
            if k == action then
                return v
            end
        end
    end
end
