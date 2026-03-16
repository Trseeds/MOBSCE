#include "MOBSCE.h"
#include "GAME.h"

int InXRange(Actor* Actor, Vector4* Box)
{
    if(
    Actor->Position.X + Actor->Dimensions.X > Box->X &&
    Actor->Position.X < Box->Z)
    {
        return(true);
    }
    return(false);
}

int InYRange(Actor* Actor, Vector4* Box)
{
    if(
    Actor->Position.Y + Actor->Dimensions.Y > Box->Y &&
    Actor->Position.Y < Box->W)
    {
        return(true);
    }
    return(false);
}

void ActorWorldCollide(Actor* Actor, Engine* Engine)
{
    if(Actor->CustomData->World)
    {
        Vector4* WV = Actor->CustomData->World;
        for(int i = 0; i < 6; i++)
        {
            //floor
            if(InXRange(Actor,&WV[i]))
            {
                if(
                Actor->Position.Y + Actor->CustomData->Velocity.Y + Actor->Dimensions.Y > WV[i].Y &&
                Actor->Position.Y + Actor->Dimensions.Y < WV[i].W)
                {
                    Actor->CustomData->Velocity.Y = 0;
                    Actor->Position.Y = WV[i].Y - Actor->Dimensions.Y;
                }
            }
            //ceiling
            if(InXRange(Actor,&WV[i]))
            {
                if(
                Actor->Position.Y + Actor->CustomData->Velocity.Y < WV[i].W &&
                Actor->Position.Y > WV[i].Y)
                {
                    Actor->CustomData->Velocity.Y = 0;
                    Actor->Position.Y = WV[i].W;
                }
            }
            //right wall
            if(InYRange(Actor,&WV[i]))
            {
                if(
                Actor->Position.X + Actor->CustomData->Velocity.X + Actor->Dimensions.X > WV[i].X &&
                Actor->Position.X + Actor->Dimensions.X < WV[i].Z)
                {
                    Actor->CustomData->Velocity.X = 0;
                    Actor->Position.X = WV[i].X - Actor->Dimensions.X;
                }
            }
            //left wall
            if(InYRange(Actor,&WV[i]))
            {
                if(
                Actor->Position.X + Actor->CustomData->Velocity.X < WV[i].Z &&
                Actor->Position.X > WV[i].X)
                {
                    Actor->CustomData->Velocity.X = 0;
                    Actor->Position.X = WV[i].Z;
                }
            }
        }
    }
}