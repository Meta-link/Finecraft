#ifndef __WORLD_H__
#define __WORLD_H__

#include "gl/glew.h"
#include "gl/freeglut.h" 
#include "engine/utils/types_3d.h"
#include "cube.h"
#include "chunk.h"

typedef uint8 NYAxis;
#define NY_AXIS_X 0x01
#define NY_AXIS_Y 0x02
#define NY_AXIS_Z 0x04

#define MAT_SIZE 10 //en nombre de chunks
#define MAT_HEIGHT 5 //en nombre de chunks
#define MAT_SIZE_CUBES (MAT_SIZE * NYChunk::CHUNK_SIZE)
#define MAT_HEIGHT_CUBES (MAT_HEIGHT * NYChunk::CHUNK_SIZE)


class NYWorld
{
public:
	NYChunk * _Chunks[MAT_SIZE][MAT_SIZE][MAT_HEIGHT];
	int _MatriceHeights[MAT_SIZE_CUBES][MAT_SIZE_CUBES];
	float _FacteurGeneration;

	NYWorld()
	{
		_FacteurGeneration = 1.0;

		//On crée les chunks
		for (int x = 0; x < MAT_SIZE; x++)
			for (int y = 0; y < MAT_SIZE; y++)
				for (int z = 0; z < MAT_HEIGHT; z++)
					_Chunks[x][y][z] = new NYChunk();

		for (int x = 0; x < MAT_SIZE; x++)
			for (int y = 0; y < MAT_SIZE; y++)
				for (int z = 0; z < MAT_HEIGHT; z++)
				{
					NYChunk * cxPrev = NULL;
					if (x > 0)
						cxPrev = _Chunks[x - 1][y][z];
					NYChunk * cxNext = NULL;
					if (x < MAT_SIZE - 1)
						cxNext = _Chunks[x + 1][y][z];

					NYChunk * cyPrev = NULL;
					if (y > 0)
						cyPrev = _Chunks[x][y - 1][z];
					NYChunk * cyNext = NULL;
					if (y < MAT_SIZE - 1)
						cyNext = _Chunks[x][y + 1][z];

					NYChunk * czPrev = NULL;
					if (z > 0)
						czPrev = _Chunks[x][y][z - 1];
					NYChunk * czNext = NULL;
					if (z < MAT_HEIGHT - 1)
						czNext = _Chunks[x][y][z + 1];

					_Chunks[x][y][z]->setVoisins(cxPrev, cxNext, cyPrev, cyNext, czPrev, czNext);
				}


	}

	inline NYCube * getCube(int x, int y, int z)
	{
		if (x < 0)x = 0;
		if (y < 0)y = 0;
		if (z < 0)z = 0;
		if (x >= MAT_SIZE * NYChunk::CHUNK_SIZE) x = (MAT_SIZE * NYChunk::CHUNK_SIZE) - 1;
		if (y >= MAT_SIZE * NYChunk::CHUNK_SIZE) y = (MAT_SIZE * NYChunk::CHUNK_SIZE) - 1;
		if (z >= MAT_HEIGHT * NYChunk::CHUNK_SIZE) z = (MAT_HEIGHT * NYChunk::CHUNK_SIZE) - 1;

		return &(_Chunks[x / NYChunk::CHUNK_SIZE][y / NYChunk::CHUNK_SIZE][z / NYChunk::CHUNK_SIZE]->_Cubes[x % NYChunk::CHUNK_SIZE][y % NYChunk::CHUNK_SIZE][z % NYChunk::CHUNK_SIZE]);
	}

	void updateCube(int x, int y, int z)
	{
		if (x < 0)x = 0;
		if (y < 0)y = 0;
		if (z < 0)z = 0;
		if (x >= MAT_SIZE * NYChunk::CHUNK_SIZE)x = (MAT_SIZE * NYChunk::CHUNK_SIZE) - 1;
		if (y >= MAT_SIZE * NYChunk::CHUNK_SIZE)y = (MAT_SIZE * NYChunk::CHUNK_SIZE) - 1;
		if (z >= MAT_HEIGHT * NYChunk::CHUNK_SIZE)z = (MAT_HEIGHT * NYChunk::CHUNK_SIZE) - 1;
		_Chunks[x / NYChunk::CHUNK_SIZE][y / NYChunk::CHUNK_SIZE][z / NYChunk::CHUNK_SIZE]->toVbo();
	}

	void deleteCube(int x, int y, int z)
	{
		NYCube * cube = getCube(x, y, z);
		cube->_Draw = false;
		cube = getCube(x - 1, y, z);
		updateCube(x, y, z);
	}

	//Création d'une pile de cubes
	//only if zero permet de ne générer la  pile que si sa hauteur actuelle est de 0 (et ainsi de ne pas regénérer de piles existantes)
	void load_pile(int x, int y, int height, bool onlyIfZero = true)
	{
		if (height < 1)
			height = 1;
		if (height >= MAT_HEIGHT_CUBES)
			height = MAT_HEIGHT_CUBES - 1;

		if (!onlyIfZero || (onlyIfZero && _MatriceHeights[x][y] == 0))
		{
			_MatriceHeights[x][y] = height;

			for (int i = 0; i <= height; i++)
			{
				if (i == height)
				{
					getCube(x, y, i)->_Type = CUBE_HERBE;
				}
				else if (i == 0)
				{
					getCube(x, y, i)->_Type = CUBE_EAU;
				}
				else
				{
					getCube(x, y, i)->_Type = CUBE_TERRE;
				}
			}

			for (int i = height + 1; i < MAT_HEIGHT_CUBES; i++)
				getCube(x, y, i)->_Type = CUBE_AIR;
		}
	}

	//Creation du monde entier, en utilisant le mouvement brownien fractionnaire
	void generate_piles(int x1, int y1,
		int x2, int y2,
		int x3, int y3,
		int x4, int y4, int prof, int profMax = -1)
	{
		if (x3 - x1 <= 1 && y3 - y1 <= 1)
			return;

		int largeurRandom = (int)(MAT_HEIGHT_CUBES / (prof*_FacteurGeneration));
		if (largeurRandom == 0)
			largeurRandom = 1;

		if (profMax >= 0 && prof >= profMax)
		{
			return;
		}

		int mid1x = (x1 + x2) / 2;
		int mid1y = (y1 + y2) / 2;
		int mid1h = (_MatriceHeights[x1][y1] + _MatriceHeights[y1][y2]) / 2;
		if ((x2 - x1) > 1)
		{
			mid1h += (rand() % largeurRandom) - (largeurRandom / 2);
			load_pile(mid1x, mid1y, mid1h);
		}
		else
			mid1h = _MatriceHeights[mid1x][mid1y];

		int mid2x = (x3 + x2) / 2;
		int mid2y = (y3 + y2) / 2;
		int mid2h = (_MatriceHeights[x3][y3] + _MatriceHeights[y1][y2]) / 2;
		if ((y3 - y2) > 1)
		{
			mid2h += (rand() % largeurRandom) - (largeurRandom / 2);
			load_pile(mid2x, mid2y, mid2h);
		}
		else
			mid2h = _MatriceHeights[mid2x][mid2y];

		int mid3x = (x3 + x4) / 2;
		int mid3y = (y3 + y4) / 2;
		int mid3h = (_MatriceHeights[x3][y3] + _MatriceHeights[y4][y4]) / 2;
		if ((x3 - x4) > 1)
		{
			mid3h += (rand() % largeurRandom) - (largeurRandom / 2);
			load_pile(mid3x, mid3y, mid3h);
		}
		else
			mid3h = _MatriceHeights[mid3x][mid3y];

		int mid4x = (x1 + x4) / 2;
		int mid4y = (y1 + y4) / 2;
		int mid4h = (_MatriceHeights[x1][y1] + _MatriceHeights[y4][y4]) / 2;
		if ((y3 - y1) > 1)
		{
			mid4h += (rand() % largeurRandom) - (largeurRandom / 2);
			load_pile(mid4x, mid4y, mid4h);
		}
		else
			mid4h = _MatriceHeights[mid4x][mid4y];

		int midx = (x1 + x3) / 2;
		int midy = (y2 + y4) / 2;
		if ((x3 - x1) > 1 && (y3 - y1) > 1)
		{
			int midh = (mid1h + mid2h + mid3h + mid4h) / 4;
			midh += (rand() % largeurRandom) - (largeurRandom / 2);
			load_pile(midx, midy, midh);
		}


		generate_piles(x1, y1, mid1x, mid1y, midx, midy, mid4x, mid4y, prof + 1, profMax);
		generate_piles(mid1x, mid1y, x2, y2, mid2x, mid2y, midx, midy, prof + 1, profMax);
		generate_piles(midx, midy, mid2x, mid2y, x3, y3, mid3x, mid3y, prof + 1, profMax);
		generate_piles(mid4x, mid4y, midx, midy, mid3x, mid3y, x4, y4, prof + 1, profMax);
	}


	void lisse(void)
	{
		/*
		int lissage[MAT_SIZE_CUBES][MAT_SIZE_CUBES];

		for (int x = 0; x < MAT_SIZE_CUBES; x++)
		{
			for (int y = 0; y < MAT_SIZE_CUBES; y++)
			{
				int nb = 0;
				for (int i = (x - 1 < 0 ? 0 : x - 1); i < (x + 1 >= MAT_SIZE_CUBES ? MAT_SIZE_CUBES - 1 : x + 1); i++)
				{
					for (int j = (y - 1 < 0 ? 0 : y - 1); j < (y + 1 >= MAT_SIZE_CUBES ? MAT_SIZE_CUBES - 1 : y + 1); j++)
					{
						lissage[x][y] += _MatriceHeights[i][j];
						nb++;
					}
				}

				if (nb)
					lissage[x][y] /= nb;
			}
		}

		//On reset les piles
		for (int x = 0; x < MAT_SIZE_CUBES; x++)
		{
			for (int y = 0; y < MAT_SIZE_CUBES; y++)
			{
				load_pile(x, y, lissage[x][y], false);
			}
		}
		*/
		int sizeWidow = 2;
		int _MatriceHeightsTmp[MAT_SIZE_CUBES][MAT_SIZE_CUBES];
		memset(_MatriceHeightsTmp, 0x00, sizeof(int)*MAT_SIZE_CUBES*MAT_SIZE_CUBES);
		for (int x = 0; x<MAT_SIZE_CUBES; x++)
		{
			for (int y = 0; y<MAT_SIZE_CUBES; y++)
			{
				//on moyenne sur une distance
				int nb = 0;
				for (int i = (x - sizeWidow < 0 ? 0 : x - sizeWidow);
				i < (x + sizeWidow >= MAT_SIZE_CUBES ? MAT_SIZE_CUBES - 1 : x + sizeWidow); i++)
				{
					for (int j = (y - sizeWidow < 0 ? 0 : y - sizeWidow);
					j <(y + sizeWidow >= MAT_SIZE_CUBES ? MAT_SIZE_CUBES - 1 : y + sizeWidow); j++)
					{
						_MatriceHeightsTmp[x][y] += _MatriceHeights[i][j];
						nb++;
					}
				}
				if (nb)
					_MatriceHeightsTmp[x][y] /= nb;
			}
		}

		//On reset les piles
		for (int x = 0; x<MAT_SIZE_CUBES; x++)
		{
			for (int y = 0; y<MAT_SIZE_CUBES; y++)
			{
				load_pile(x, y, _MatriceHeightsTmp[x][y], false);
			}
		}

	}




	void init_world(int profmax = -1)
	{
		_cprintf("Creation du monde %f \n", _FacteurGeneration);

		srand(timeGetTime());

		//Reset du monde
		for (int x = 0; x < MAT_SIZE; x++)
			for (int y = 0; y < MAT_SIZE; y++)
				for (int z = 0; z < MAT_HEIGHT; z++)
					_Chunks[x][y][z]->reset();
		memset(_MatriceHeights, 0x00, MAT_SIZE_CUBES*MAT_SIZE_CUBES*sizeof(int));

		//On charge les 4 coins
		load_pile(0, 0, MAT_HEIGHT_CUBES / 2);
		load_pile(MAT_SIZE_CUBES - 1, 0, MAT_HEIGHT_CUBES / 2);
		load_pile(MAT_SIZE_CUBES - 1, MAT_SIZE_CUBES - 1, MAT_HEIGHT_CUBES / 2);
		load_pile(0, MAT_SIZE_CUBES - 1, MAT_HEIGHT_CUBES / 2);

		//On génère a partir des 4 coins
		generate_piles(0, 0,
			MAT_SIZE_CUBES - 1, 0,
			MAT_SIZE_CUBES - 1, MAT_SIZE_CUBES - 1,
			0, MAT_SIZE_CUBES - 1, 1, profmax);

		for (int i = 0; i < 3; i++)
			lisse();

		for (int x = 0; x < MAT_SIZE; x++)
			for (int y = 0; y < MAT_SIZE; y++)
				for (int z = 0; z < MAT_HEIGHT; z++)
					_Chunks[x][y][z]->disableHiddenCubes();

		add_world_to_vbo();
	}

	NYCube * pick(NYVert3Df  pos, NYVert3Df  dir, NYPoint3D * point)
	{
		return NULL;
	}

	//Boites de collisions plus petites que deux cubes
	NYAxis getMinCol(NYVert3Df pos, float width, float height, float & valueColMin)
	{
		int x = (int)(pos.X / NYCube::CUBE_SIZE);
		int y = (int)(pos.Y / NYCube::CUBE_SIZE);
		int z = (int)(pos.Z / NYCube::CUBE_SIZE);

		int xNext = (int)((pos.X + width / 2.0f) / NYCube::CUBE_SIZE);
		int yNext = (int)((pos.Y + width / 2.0f) / NYCube::CUBE_SIZE);
		int zNext = (int)((pos.Z + height / 2.0f) / NYCube::CUBE_SIZE);

		int xPrev = (int)((pos.X - width / 2.0f) / NYCube::CUBE_SIZE);
		int yPrev = (int)((pos.Y - width / 2.0f) / NYCube::CUBE_SIZE);
		int zPrev = (int)((pos.Z - height / 2.0f) / NYCube::CUBE_SIZE);

		if (x < 0)	x = 0;
		if (y < 0)	y = 0;
		if (z < 0)	z = 0;

		if (xPrev < 0)	xPrev = 0;
		if (yPrev < 0)	yPrev = 0;
		if (zPrev < 0)	zPrev = 0;

		if (xNext < 0)	xNext = 0;
		if (yNext < 0)	yNext = 0;
		if (zNext < 0)	zNext = 0;

		if (x >= MAT_SIZE_CUBES)	x = MAT_SIZE_CUBES - 1;
		if (y >= MAT_SIZE_CUBES)	y = MAT_SIZE_CUBES - 1;
		if (z >= MAT_HEIGHT_CUBES)	z = MAT_HEIGHT_CUBES - 1;

		if (xPrev >= MAT_SIZE_CUBES)	xPrev = MAT_SIZE_CUBES - 1;
		if (yPrev >= MAT_SIZE_CUBES)	yPrev = MAT_SIZE_CUBES - 1;
		if (zPrev >= MAT_HEIGHT_CUBES)	zPrev = MAT_HEIGHT_CUBES - 1;

		if (xNext >= MAT_SIZE_CUBES)	xNext = MAT_SIZE_CUBES - 1;
		if (yNext >= MAT_SIZE_CUBES)	yNext = MAT_SIZE_CUBES - 1;
		if (zNext >= MAT_HEIGHT_CUBES)	zNext = MAT_HEIGHT_CUBES - 1;

		//On fait chaque axe
		NYAxis axis = 0x00;
		valueColMin = 10000.0f;

		//On verif tout les 4 angles de gauche
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid())
		{
			float depassement = ((xPrev + 1) * NYCube::CUBE_SIZE) - (pos.X - width / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_X;
			}
		}

		float depassementx2 = (xNext * NYCube::CUBE_SIZE) - (pos.X + width / 2.0f);

		//On verif tout les 4 angles de droite
		if (getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			float depassement = (xNext * NYCube::CUBE_SIZE) - (pos.X + width / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_X;
			}
		}

		float depassementy1 = (yNext * NYCube::CUBE_SIZE) - (pos.Y + width / 2.0f);

		//On verif tout les 4 angles de devant
		if (getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			float depassement = (yNext * NYCube::CUBE_SIZE) - (pos.Y + width / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_Y;
			}
		}

		float depassementy2 = ((yPrev + 1) * NYCube::CUBE_SIZE) - (pos.Y - width / 2.0f);

		//On verif tout les 4 angles de derriere
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid())
		{
			float depassement = ((yPrev + 1) * NYCube::CUBE_SIZE) - (pos.Y - width / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_Y;
			}
		}

		//On verif tout les 4 angles du haut
		if (getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			float depassement = (zNext * NYCube::CUBE_SIZE) - (pos.Z + height / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_Z;
			}
		}

		//On verif tout les 4 angles du bas
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid())
		{
			float depassement = ((zPrev + 1) * NYCube::CUBE_SIZE) - (pos.Z - height / 2.0f);
			if (abs(depassement) < abs(valueColMin))
			{
				valueColMin = depassement;
				axis = NY_AXIS_Z;
			}
		}

		return axis;
	}


	void render_world_vbo(void)
	{
		for (int x = 0; x < MAT_SIZE; x++)
			for (int y = 0; y < MAT_SIZE; y++)
				for (int z = 0; z < MAT_HEIGHT; z++)
				{
					glPushMatrix();
					glTranslatef((float)(x*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE), (float)(y*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE), (float)(z*NYChunk::CHUNK_SIZE*NYCube::CUBE_SIZE));
					_Chunks[x][y][z]->render();
					glPopMatrix();
				}
	}

	void add_world_to_vbo(void)
	{
		int totalNbVertices = 0;

		for (int x = 0; x < MAT_SIZE; x++)
			for (int y = 0; y < MAT_SIZE; y++)
				for (int z = 0; z < MAT_HEIGHT; z++)
				{
					_Chunks[x][y][z]->toVbo();
					totalNbVertices += _Chunks[x][y][z]->_NbVertices;
				}

		Log::log(Log::ENGINE_INFO, (toString(totalNbVertices) + " vertices in VBO").c_str());
	}

	void render_world_old_school(void)
	{
		glEnable(GL_COLOR_MATERIAL);
		glPushMatrix();
		for (int i = 0; i < MAT_SIZE_CUBES; i++)
		{
			for (int j = 0; j < MAT_SIZE_CUBES; j++)
			{
				for (int k = 0; k < MAT_HEIGHT_CUBES; k++)
				{
					NYCube* cube = getCube(i, j, k);
					if (cube->_Draw && cube->isSolid())
					{
						glPushMatrix();
						glTranslatef(i, j, k);
						switch (cube->_Type)
						{
						case CUBE_TERRE:
							glColor3ub(100, 70, 30);
							break;
						case CUBE_EAU:
							glColor3ub(0, 100, 255);
							break;
						case CUBE_HERBE:
							glColor3ub(50, 180, 70);
							break;
						default:
							glColor3ub(0, 0, 0);
							break;
						}

						glutSolidCube(1);
						glPopMatrix();
					}
				}
			}
		}
		glPopMatrix();
		glDisable(GL_COLOR_MATERIAL);
	}
};



#endif