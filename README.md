# /----------------
# /BAREBONE
# /----------------

a very simple game engine made in C, it uses the terminal as the display. (comes with bounCe as example project)



# /----------------
# /HOW TO USE?
# /----------------

define the game constants as you wish.
to make a new entity: choose a code for it, insert its behaviour inside the switch in entity_update with its code as the case, set its appearance inside the correspondent entity_char array position.
to make a new room: inside levels, insert a new array with the size of LEVEL_WIDTH * LEVEL_HEIGHT, inside that array you must define each new entity position.
