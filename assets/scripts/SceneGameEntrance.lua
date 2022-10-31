SceneGameEntrance = {
    layers = {
        [0] = "outside_parking",
    },

    bgMusic = "parking",
    firstDialog = {
            "Ugh, I forgot my car keys. I should head back in before Gorm closes the building for the night.",
            "They should be in my office on the top floor."
        },

        actionItems = {
            umbrella = {
                isVisible = true,
                sprite = "umbrella",
                rect = { 5, 184, 26, 10 },
                dialogs = {
                    look = "Why would someone leave a perfectly good umbrella here?",
                },
                onTake = function(self, scene)
                        self.isVisible = false
                        addDialog("Well, might as well take it. It's not gonna be any use just sitting here.")
                        insertInventory("Umbrella")
                end,
                sort = 1

            },

            carani = {
                dialogs = {
                    look = "Electric cars have come a long way. Maybe one day they'll shoot laser beams or something!"
                },

                onUse = function (self, scene)
                    if holdingItem == "Car keys" then
                        scene.actionItems.carani.isVisible = true
                        PlayWav("car")
                        queueStart("end1", function()
                        end)
                        queueDestination("SceneEnd", false)
                    else
                        addDialog("Can't leave before I find my car keys..")
                    end
                end,
                animatedSprite = "end1",

                isAutoStart = false,
                rect = {69, 140, 155, 84},
                sort = 1
            },


            entrance = {
                rect= {69,71,78,70},
                dialogs = {
                    look = "The big storm last night seems to have broken some plumbing.",
                },
                isMinimap = false,
                minimap = { 85, 297 }, --Top center
                destination = "SceneLobby",
                onOpen = function(self, scene)
                    if containsInventory("Car keys") then
                        addDialog("No need to go back inside again.")
                    else
                        if holdingItem == "Umbrella" then
                            moveToDestination(self.destination)
                        else
                            addDialog("I don't want to get wet.")
                        end

                    end

                end,
                onUse = function(self, scene)
                    if holdingItem == "Umbrella" then
                       self.isMinimap = true
                       addDialog("I should be able to enter the building now.")
                    else
                       addDialog("I can't use that.")
                    end

                end,
                sort = 3,
            },
            streetlight = {
                animatedSprite = "streetlight",
                rect = {0, 51, 49, 153},
                sort = 4,
                dialogs = {
                    look = "This streetlight could use some maintenance."
                }
            },
            rain = {
                rect = {0,0,224,224},
                animatedSprite = "rain",
                isActive = false,
            }
        }

}

