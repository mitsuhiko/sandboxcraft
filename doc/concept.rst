Game Concept
============

The working title for this game is "Sandbox Craft".  That was the name I
chose as based for the prefix of the C files when I started that project
and it stems from the Minecraft engine clone it started being.

From that point onwards however, the focus shifted to a game that is more
about exploring than creating.

Key Influences
--------------

Before I outline what the game is about I want to list the major influeces
here first and why they came to be.

1.  **Minecraft**.  That should be obvious as the game is also based on
    blocks.  In fact Minecraft was the main inspiration for me to get back
    into trying game development as it showed that people are okay with
    simple graphics for as long as the gameplay is interesting.  Secondly
    because it shows nicely that exploring random generated worlds is
    interesting for as long as there is random stuff to find.

2.  **Pikmin**.  This might sound interesting but Pikmin is actually the
    base of the majority of the game concept.  And that would be having
    small minions that are controlled by walking up to them and giving
    them commands.  The main difference here however is that the minions
    in this game concept are not as stupid and simple minded as pikmins
    are, however they cannot be directly controlled and there also is no
    overhead view.  You are commanding the minions from upclose.

Setting
-------

The game plays in a fantastic world with large floating islands high above
the sky of a poisonous world.  These islands are between 200 and 500
meters in width and height.  They form half spheres and have different
environmental features.  Whenever a player discovers an island it is
picked from a bunch of different algorithms (Deserts, Frozen worlds with
icebergs, meadows, high mountains, large hills and rivers, large valleys
etc.)

The whole game is held steam punk inspired when it comes to the player
available technology.  This not only serves as the main inspiration for
assets but also has a huge impact on how the game world exploration works.
By leaving airships as only means of cross island transportation one can
facilitate the slow travel to load the complex islands out and into
memory.

Setup
-----

The whole game is based on "the universe", "a world" and "a chunk":

the universe:
    The universe is the space you can travel to.  It is in fact the
    atmosphere of a poisonous planet and flipped at a specific coordinate.
    Flying out on one of the edge makes you come back on the other edge.
    While it's never shown and not implemented that one, one would have to
    think of the universe as a band of a sphere with a height of around 8
    kilometers that spans round the planet.

the world:
    The universe is then split into worlds.  A world is a block of the
    size 8.192 cubic meters divided into chunks of 64 cubic meters.

a chunk:
    Chunks themselves are octrees of 64**3 cubic meters and made out of
    equally many blocks.  Each block is exactly one cubic meter and can be
    made of one specific material.  It is not possible to have more than
    one block in a specific slot of the tree, but some blocks might only
    fill half of the slot.

Chunk Swapping
--------------

Because the world is that humongous we need a couple of tricks to be able
to keep it in memory and alive.  First of all, the game is supposed to
support cooperative play with more than one player.  Secondly the world is
managed by a central server that does the world generation and sends out
chunks to the players as necessary.

This has a few implications that are important to keep in mind when
designing the system:

-   minions can move around on the world and modify it.  This will also
    (to a point) have to happen when nobody is in the chunk and there is
    no other reason to keep it in memory.  Even if the world is divided
    into chunks of a small size it still means that we will waste tons of
    memory and have to seen countless data between servers and clients.

-   The naive memory consumption for a single chunk would be around half a
    megabyte.  Thus fully filled world block would be around 4 GB.  This
    pretty quickly shows that one needs to find better ways to keep this
    in memory but also that it will be necessary to a) swap the data to
    and from the disc and b) find a way to cheat activity on parts of the
    map where nobody but minions are.

Generally if a chunk is stored as a vbo, the worst case memory consumption
would go down quite a bit to (assuming 2 byte for block data and 4 byte
for pointers) 2 * 4 * 8 * log(8192) [that sounds wrong].  The best case
memory consumption would be about 8 bytes if the pointer to the vbo is
stored as well.  Because the majority of the world is made of the same
materials (air, dirt etc.) the octree sounds like the best approach.
Inside the world the nodes could be stored in either a hashmap indexed by
(x, y, z) of the chunk address or as an octree as well.  In theory the
hashmap has better access times but in practice it will probably also have
the higher memory consumption.  This will require some tests.

The actual worlds are stored in a sparse 3d matrix that is implemented as
a hashtable indexed by (x, y, z) of the world coordinates.  Worlds are
always separated by exactly one world size into either direction.  This is
done to keep the further apart by definition and more importantly to give
us some space to do load time optimizations by using the travel time to
shovel data in and out of memory.

The process of swapping a chunk to disc on the server means two things:

1.  the chunk is stored to the filesystem.  The actual file format is
    open for discussion but it has to be efficient enough for writing
    often and probably not dump too many files there.  It might be a good
    idea to store that kind of stuff into a deflated zip file.

2.  The non-world state of the chunk is stored as well together with the
    timestamp.  The non-world state would be buildings in progress,
    minions working on these buildings etc.  Generally minions are always
    assigned the chunk their action target is located in.  If their action
    spans more than one chunk both chunks are swapped out together.  For
    instance a building that spans three chunks will have the worker
    minions being assigned to these three chunks as well.  When a minion
    is not on the target chunk when the chunk is swapped out, it's flag is
    changed and it will move to the target chunk and disappear.  Because
    minions can never be controlled directly that is probably okay.

    Secondly this will also mean that due to latency and calculation
    tricks the minion population will never be exact.  Thus the game
    design has to take this volatile count into consideration.  The
    minions are more like RCT park visitors than AOE workers with a
    specific count.

Farming and Mining
------------------

The world is made of blocks of specific types, generated by various perlin
noise based world generators.  When the blocks are stored in the world as
resources they are just that: resources.  Mining or farming them will
never yield a block of the same type, but a derived type.  Some of these
might look very much alike, but there is a fundamental difference: raw
types are automatically collectable by minions whereas their derived works
will not.  This is a very basic protection against minions mining
buildings.  For instance if a minion is mining stone it will yield
cobblestone.  For a building that cobblestone is converted into fine
stone or something similar.  That way the same material can be easily kept
apart.

A player can mine as well, but from a specific point onwards it makes most
sense to let the minions to the mining as the process takes a much longer
period of time than in Minecraft.

Buildings
---------

Once a player found the blueprint for a building they can ask their
minions to erect it.  Each building is made of a key stone and some other
materials in very specific locations.  Buildings are pre-made.  One can in
theory create the building stone by stone from scratch and the game code
will recognize that it's that specific building and assign it the
requested function, but this exists mainly for the reason of repairing
accidentally destroyed buildings than to create them from scratch.

For the engine a building are stones that share an action ID.  Whenever a
block is added next to one block with an already existing action ID it is
checked in a central location for whatever action corresponds with it.  On
top of that, blocks that share the same action ID are swapped out in it's
entirety, never just one chunk of it.  The idea of these action IDs is
that they can be used to keep buildings apart from the point of view from
the engine and to assign them dynamic actions.

For instance an incubator will be able to spawn minions so whenever the
player performs an action on the building the engine will check if the
block hit belongs to a building with an action assigned.  If it finds an
action and sees that this is an incubator, it will automatically open the
incubator configuration panel.

Buildings that are detected as buildings are given a distinct look
(contour, different shading / lighting etc.) to help the user find out
when a building was destroyed.  The moment a building gets accidentally
destroyed because of a removed block a flag is flipped in the action
entry.  Next time the user right clicks the building he is offered the
ability to command minions to it to repair it.

Minion Controlling
------------------

Minions are controlled by getting commands from the player.  The player
approaches a leader minion and gives it a command and the number of
minions that leader should allocate to complete it.  A leader minion is
always assigned a target building or chunk it's operating in.  If the
target is a chunk and the action ended in the chunk without result the
leader minion will automatically try the chunks nearby.  If it can't
execute it's commands there, it will retreat with all worker minions to
the base.

In case the leader minion and his helpers are swapped out they will assume
to operate in nearly ideal environment.  A general factor for is applied
to the time spend swapped out (for instance 0.8) to account for potential
problems.  When there chunks are swapped in the changes to the
environment are calculated and applied.
