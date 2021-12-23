# WASM-4 RPG
RPG for WASM-4 fantasy game console

![Game2](https://github.com/jzeiber/wasm4-rpg/raw/main/images/screenshot1.png "Gameplay")
![Game2](https://github.com/jzeiber/wasm4-rpg/raw/main/images/screenshot2.png "Gameplay")
![Game2](https://github.com/jzeiber/wasm4-rpg/raw/main/images/screenshot3.png "Gameplay")
![Game2](https://github.com/jzeiber/wasm4-rpg/raw/main/images/screenshot4.png "Gameplay")

# Play
https://jzeiber.github.io/wasm4-rpg/bundle/

# How to Play

## Controls

Use the arrow keys/gamepad direction buttons to move around
X/Gamepad button 1 to execute selection
Z/Gamepad button 2 to change selection

The main menu provides 2 "slots" to use to save 2 different games.  Use the arrow keys/gamepad direction button to select the slot and press X or gamepad button 1 to choose the slot.  Depending on if you already had a game saved, you can continue a game, start a new game, delete the game, or cancel.

The settings menu provides a way to change the key/button press repeat rate during gameplay.  The repeat delay affects how long after you first press a key that the key will start repeating, and the repeat rate affects how quickly the press will repeat after that.

## Limitations

Because of the limited storage space available, the following limitations are in place

During gameplay 20 monsters may be "alive" at once.  When the game is saved only the closest 5 monsters to the player are saved.
Only 5 items may be placed on the ground at once.  If an item is dropped with 5 items on the ground, one of the items currently on the ground will be replaced with the newly dropped item.  Additionally moving too far away from a dropped item will cause it to be removed from the ground.

## Gameplay

Your character is in the middle of the screen.  Use the arrows/direction pad to move around the map.  Your health appears in the top right of the screen in green, and your progress to the next level appears in blue to the right of the health.  Any items on the ground where you are standing will show up in the lower right.  Items that show up as green have a stat that is better than the item you currently have equipped.

Monsters will randomly roam around the map.  To attack a monster, move next to them and press the arrow/direction pad in their direction.  You will gain experience as you kill monsters.  Some monsters may also drop useful items from time to time.

Towns scatter the landscape.  Visit a town to get a quest.  If an active quest has a location that you need to go to, white arrows will show up on the border of the screen to show you the direction.  Upon completion of a quest you will usually receive an item.

The bottom of the screen contains a menu bar of actions/screens that can be selected.  Use Z/button 2 on the gamepad to change the selected item and X/button 1 to perform the action.

- The save icon will save the current game.
- The journal icon will open a screen that lists the current quests you have accepted.
- The map icon will open a map with quest locations marked on it.
- The campfire icon will cause your character to rest as long as no monsters are around.  You may be interrupted during your rest by aggressive monsters if resting in the open.  Resting in a town is safest.
- The character icon will open a screen with your character stats.
- The loot icon will open your inventory.
- The grab icon will pick up an item on the ground directly beneath you.
- The home icon will exit the current game and go back to the main menu.

## Inventory

You have space for 20 items in your inventory.  The items currently in your inventory show up at the top of the screen.  Items in green are currently equipped, and items in blue are quest items.  Use the arrow keys/dirction pad to move around the inventory slots.

The middle of the screen will show stats about the currently selected item.

At the bottom of the inventory screen is a menu bar of actions that can be performed.  Use Z/button 2 on the gamepad to change the selected item and X/button 1 to perform the action.

- The return icon will close the inventory.
- The equip icon will toggle the currently selected item to be equipped/unequipped.  This will only work for equipable items.
- The consume icon will consume the currently selected item.  This will only work for consumable items.
- The drop icon will drop the currently selected item on the ground.
- The move left/right icons will swap the currently selected item with the item to the left/right in the inventory.

## Tips

Save the game often.

Try to gather a full set of armor as soon as possible.

Rest after each fight, and in a town if possible, especially early in the game when you don't have a lot of hit points.

Lure single monsters away to fight rather than attacking a group of monsters.

The sea can be dangerous, with no safe place to rest.  Make sure you are equipped to handle any long sea voyages.
