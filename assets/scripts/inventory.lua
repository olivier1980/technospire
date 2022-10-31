inventory = {

}

holdingItem = nil
magicInterface = false

clearInventory = function ()
    inventory = {}
end

function removeInventory(invItem)
    local pos = 0
    for i = 1, #(inventory) do
        if inventory[i] == invItem then
            pos = i
        end
    end
    if pos > 0 then
        table.remove(inventory, pos)
    end

end

function containsInventory(item)
    local found = false
    for k,v in pairs(inventory) do
        if v == item then
            --print("found")
            found = true
        end
    end

    return found
end

function insertInventory(invItem)
    --print ("insert inv ", invItem);

    if (#inventory) == 0 then
        --print ("table insert");
        table.insert(inventory, invItem)
    end

    local found = false
    for k,v in pairs(inventory) do
        if v == invItem then
           -- print("found")
            found = true
        end
    end

    if not found then
        --print("another insert")
        table.insert(inventory, invItem)
    end

    --print ("done");
end
