assets = {
    title = { 0, 0, 512, 464 },
    title2 = { 512, 0, 512, 464 },
    main = { 0, 464, 512, 464 },
    main_gui = { 512, 688, 370, 177 },
    gui_magic = { 912, 928, 88, 145 },
    magic_orb = { 640, 880, 43, 37 },
    magic_shield = { 688, 880, 52, 37 },
    magic_light = { 752, 880, 78, 36 },


    death = { 0, 2128, 224, 224 },

    --main_gui
    gui_open = { 752, 576, 66, 21 },
    gui_take = { 752, 608, 66, 21 },
    gui_use = { 752, 640, 49, 21 },

    --load
    trash = { 800, 512, 18, 18 },

    --scene
    outside_parking = { 512, 464, 224, 224 },
    umbrella = { 750, 467, 22, 7 },

    lobby = { 0, 928, 224, 224 },
    lobby_gorm = { 224, 928, 224, 224 },
    ticket = { 776, 467, 18, 16 },

    old_hallway = { 448, 928, 224, 224 },

    old_gorm = { 672, 928, 224, 224 },
    gorm_face = { 750, 526, 36, 29 },

    --staircase
    staircase = { 0, 1152, 224, 224 },
    staircase_brick = { 752, 496, 29, 23 },
    staircase_loosebrick = { 800, 496, 19, 9 },
    powercore = { 800, 536, 10, 6 },

    --basement
    basement = { 448, 1152, 224, 224 },
    basement2 = { 0, 1824, 224, 224 },
    chest_open = { 528, 880, 94, 24 },
    hidden_block = { 832, 896, 23, 14 },
    basement_key = { 832, 880, 15, 5 },

    --doors
    doors = { 224, 1152, 224, 224 },
    doors_river = {240, 1952, 180, 24},
    doors2 = { 672, 1152, 224, 224 },
    lock1 = {912, 1296, 9, 9},
    lock2 = {928, 1296,11,11},
    lock3 = {944, 1296,7,7},
    lock4 = {960, 1296,7,8},

    --shop
    merchant = { 0, 1376, 224, 224 },
    staff = { 832, 464, 117, 173 },
    merchant_key = { 960, 464, 34, 32 },

    --armoury
    armory = { 448, 1376, 224, 224 },
    knife = { 800, 544, 14, 12 },
    star = { 960, 544, 11, 15 },
    armory_key = {67, 1829, 15, 6},
    armory_cage_empty = {0, 1840, 67, 113},
    armory_cage_key = {1072, 2048, 67,113},

    --dining
    dining = { 672, 1376, 224, 224 },
    helldining = { 0, 1600, 224, 224 },
    dining_knife = { 960, 512, 12, 26 },
    dining_keycard = { 960, 496, 24, 15 },

    --cliff
    cliff = { 224, 1600, 224, 224 },
    clouds = { 896, 688, 86, 117 },
    stones = { 912, 1088, 87, 118 },

    --fountain
    fountain = { 448, 1600, 224, 224 },
    fountain2 = { 672, 1600, 224, 224 },
    fountain_key = { 912, 1232, 19, 13 },

    --animated
    pink_button = { 752, 672, 9, 9 },
    security = {880,880, 30,15},
    streetlight = {912, 1312, 49, 153},
    elevator = {240, 1840, 77, 101},
    bossdoor = {432, 2048, 57, 64},

    boss_bg = {448,2128,224,224},
    boss_grass = {0, 2048, 224, 80},
    boss_grass_highlight = {848,2176,145,61},
    boss_platform = {688, 2128, 140, 76},
    boss_platform_highlight = {848, 2128, 78, 25},
    boss_boss = {688,2224 ,145 ,90 },
    boss_umbrella = {912, 1520, 108, 98},
    umbrellashield = {11, 2402,113,100},
    bossdeath = {16, 2544, 148, 97 },
    sunrise = {1072, 0, 224, 224},

    boss_shield_ani1 = {288, 2384, 131, 89},
    boss_shield_ani2 = {848,2256,131, 89},

    cliff1 = {240, 1984, 116, 33},
    cliff2 = {240, 2032, 168, 40},
    cliff3 = {240, 2080, 154, 26},
    cliff_platform = {672, 1824, 224, 224},
    entrance_clouds = {416, 1200,202,26},

    keys = {896, 912, 13, 7},

    end1 = {1312, 0, 155, 84},
    rain = {1312, 1120, 224,224},

    elevator2 = {1700, 0, 224,224}
}

paraSprites = {
    doors_river = {
        rect = {0, 117, 180, 24},
        speed = 30
    },

    cliff1 = {
        rect = {0, 170, 116, 33},
        speed = 5
    },
    cliff2 = {
        rect = {0, 184, 168, 40},
        speed = 10,
        isReverse = true
    },
    cliff3 = {
        rect = {0, 202, 154, 26},
        speed = 15
    },
    entrance_clouds = {
        rect = {0, 8, 224,26},
        speed = 4
    },

}

mode7Sprites = {

    boss_clouds = {
        x = -104,
        fFoVDivider = 11.6,
        sampleHeight = 1383,
        sampleWidth = 239,
        horizon = -377,
        height = 45,
        speed = 741,
        sprite = "clouds3.png",
        angle = 4
    },

    boss_clouds2 = {
        x = 0,
        fFoVDivider = 3.6,
        sampleHeight = 1383,
        sampleWidth = 239,
        horizon = -438,
        height = 44,
        speed = 282,
        sprite = "clouds2.png",
        angle = 15
    }

}

animatedSprites = {
    pink_button = {
        --animatedSprite = "pink_button",
        --isVertical = true,--should be read in assetmanager
        --pattern = "0:120,1:120", --position of frame and duration
        framespeed = 1,
        --loop = 2,
        numFrames = 2, --should be read in assetmanager
    },
    security = {
        --animatedSprite = "security",
        pattern = "0:2600,1:100,2:2600,1:100",
        --framespeed = 3,
        numFrames = 3
    },
    armory_cage_empty = {
        numFrames = 3,
        pattern = "0:600,1:400,2:600,1:400",
        framespeed = 3,
    },
    armory_cage_key = {
        numFrames = 3,
        pattern = "0:600,1:400,2:600,1:400",
        framespeed = 3,
    },
    streetlight = {
        --animatedSprite = "streetlight",
        pattern = "0:2600,1:100,0:1600,1:50, 0:250, 1:100",
        --framespeed = 3,
        numFrames = 2
    },
    elevator = {
        --animatedSprite = "elevator",
        --isVertical = true,--should be read in assetmanager
        --pattern = "0:120,1:120", --position of frame and duration
        framespeed = 5,
        loop = 1,
        numFrames = 5, --should be read in assetmanager
    },
    bossdoor = {
        framespeed = 5,
        loop = 1,
        numFrames = 5,
    },
    death = {
        numFrames = 2,
        framespeed = 10
    },
    umbrellashield = {
        numFrames = 2,
        framespeed = 5
    },
    boss_shield_ani1 = {
        numFrames = 10,
        framespeed = 5,
        loop = 1

    },
    boss_shield_ani2 = {
        numFrames = 3,
        framespeed = 5
    },
    bossdeath = {
        numFrames = 11,
        framespeed = 4,
        loop =1
    },
    sunrise = {
        numFrames = 9,
        framespeed = 3,
        loop = 1,
        isVertical = true
    },

    end1 = {
        numFrames = 14,
        loop = 1,
        isVertical = true,
        framespeed = 5,
    },
    rain = {
        numFrames = 2,
        framespeed = 4,
        isVertical = true

    },
    elevator2 = {
        numFrames = 16,
        pattern = "0:4000,1:4000,2:4000,3:4000,4:700,5:120,6:120,7:120,8:120,9:120,10:120,11:120,12:120,13:120,14:4000,15:200",
        isVertical = true,
        loop = 1
    },
    ending = {
        numFrames = 24,
        isVertical = true,
        framespeed = 10,
    }

}
