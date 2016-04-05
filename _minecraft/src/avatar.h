#ifndef __AVATAR__
#define __AVATAR__

#include "engine/utils/types_3d.h"
#include "engine/render/camera.h"
#include "world.h"

class NYAvatar
{
	public :
		NYVert3Df Position;
		NYVert3Df Speed;

		NYVert3Df MoveDir;
		bool Move;
		bool Jump;
		float Height;
		float Width;
		bool avance;
		bool recule;
		bool gauche;
		bool droite;
		bool Standing;

		NYCamera * Cam;
		NYWorld * World;

		NYAvatar(NYCamera * cam,NYWorld * world, NYVert3Df position = NYVert3Df(0,0,0))
		{
			Position = position;
			Height = 10;
			Width = 3;
			Cam = cam;
			avance = false;
			recule = false;
			gauche = false;
			droite = false;
			Standing = false;
			Jump = false;
			World = world;
		}


		void render(void)
		{
			glutSolidCube(Width/2);
		}

		void update(float elapsed)
		{
			NYVert3Df force = NYVert3Df(0, 0, -1) * 100.0f;

			if (!Standing)
				Jump = false;

			if (Standing)
			{
				if (avance)
				{
					force += Cam->_Direction * 400;
				}
				else if (recule)
				{
					force += Cam->_Direction * -400;
				}
				if (gauche)
				{
					force += Cam->_NormVec * -400;
				}
				if (droite)
				{
					force += Cam->_NormVec * 400;
				}
			}
			else
			{
				if (avance)
				{
					force += Cam->_Direction * 50;
				}
				else if (recule)
				{
					force += Cam->_Direction * -50;
				}
				if (gauche)
				{
					force += Cam->_NormVec * -50;
				}
				if (droite)
				{
					force += Cam->_NormVec * 50;
				}
			}

			if (Jump)
			{
				force += NYVert3Df(0, 0, 1) * 50.0f / elapsed; //(impulsion, pas fonction du temps)
				Jump = false;
			}

			Speed += force * elapsed; 

			//SPEED CAP
			NYVert3Df horSpeed = Speed;
			horSpeed.Z = 0;
			if (horSpeed.getSize() > 70.0f)
			{
				horSpeed.normalize();
				horSpeed *= 70.0f;
				Speed.X = horSpeed.X;
				Speed.Y = horSpeed.Y;
			}

			NYVert3Df oldPosition = Position;
			Position += (Speed * elapsed);

			Cam->moveTo(Position);

			for (int i = 0; i < 3; i++)
			{
				float valueColMin = 0;
				NYAxis axis = World->getMinCol(Position, Width, Height, valueColMin);
				if (axis != 0)
				{
					valueColMin = max(abs(valueColMin), 0.0001f) * (valueColMin > 0 ? 1.0f : -1.0f);
					if (axis & NY_AXIS_X)
					{
						Position.X += valueColMin;
						Speed.X = 0;
					}
					if (axis & NY_AXIS_Y)
					{
						Position.Y += valueColMin;
						Speed.Y = 0;
					}
					if (axis & NY_AXIS_Z)
					{
						Speed.Z = 0;
						Position.Z += valueColMin;
						Speed *= pow(0.01f, elapsed);
						Standing = true;
					}
				}
			}

			avance = false;
			recule = false;
			gauche = false;
			droite = false;
		}
};

#endif