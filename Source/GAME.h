/*
IMPORTANT!!
    For a real game you're going to need more data attatched to the Actor and Sprite
    structs than i am giving you. My solution is to include the two following structs
    in Sprites and Actors. You MUST leave this delcaration in tact, even if you don't
    use it, or else compilation will fail. Fill with your own data and access via
    Sprites/Actors->CustomData.YourMemberHere.
    Really this is the only thing that needs to go in this header. You are welcome to
    use it for your game, but you can not remove these declarations unless you remove
    them from the Sprite and Actor structs too.
*/

typedef struct CustomSpriteData {
    unsigned char Byte;
} CustomSpriteData;

typedef struct CustomActorData {
    unsigned char Byte;
} CustomActorData;