#pragma once
#include <src/Utils/Hashing/xxHash.h>
#include <glm/ext/vector_float2.hpp>
#include <vector>

class PerlinNoise {
public:
	//returns from 0-1
	static std::vector<std::vector<float>> GetLattice2D(int x, int y, int width, int numOctaves, float lacunarity, float persistence) {
		//lacunarity > 1.0
		// persistence = [0.3,1.0]
		std::vector<std::vector<float>> res(width, std::vector<float>(width,0));


		float invLacunarity = 1.f / lacunarity;

		float amplitude = 1;
		float wavelength = 1;
		float frequency = 1;
		float maxAmplitude = 0;

		for (int i = 0; i < numOctaves; i++) {

			for (int xi = 0; xi < width; xi++) {
				float u = xi / (float)(width);
				for (int zi = 0; zi < width; zi++) {
					float v = zi / (float)(width);
					res[xi][zi] += GetNoise2D((x+u) * frequency, (y+v) * frequency, i) * amplitude;
				}
			}
			maxAmplitude += amplitude;
			amplitude *= persistence;
			wavelength *= invLacunarity;
			frequency *= lacunarity;

		}
		float invMaxAmplitude = 1.f / maxAmplitude;
		for (int xi = 0; xi < width; xi++) {
			for (int zi = 0; zi < width; zi++) {
				res[xi][zi] *= invMaxAmplitude;
				res[xi][zi] += 1;
				res[xi][zi] /= 2;
			}
		}

		return res;

	}
	static float GetNoise2D(float x, float y,int octave = 0) {
		//std::cout << x << '-' << y << std::endl;

		xxHash x0, x1;
		xxHash h00, h01, h10, h11;
		glm::vec2 d00, d01, d10, d11;
		float p00, p01, p10, p11;
		glm::vec2 g00, g01, g10, g11;

		float u = x - std::floor(x);
		float v = y - std::floor(y);
		int floorX = std::floor(x);
		int floorY = std::floor(y);


		x0 = x0.eat(floorX) + octave;
		x1 = x1.eat(floorX + 1) + octave;

		h00 = x0.eat(floorY);
		h01 = x0.eat(floorY + 1);
		h10 = x1.eat(floorY);
		h11 = x1.eat(floorY + 1);

		g00 = glm::vec2(h00.Floats01A() > 0.5f ? 1 : -1, h00.Floats01B() > 0.5f ? 1 : -1);
		g01 = glm::vec2(h01.Floats01A() > 0.5f ? 1 : -1, h01.Floats01B() > 0.5f ? 1 : -1);
		g10 = glm::vec2(h10.Floats01A() > 0.5f ? 1 : -1, h10.Floats01B() > 0.5f ? 1 : -1);
		g11 = glm::vec2(h11.Floats01A() > 0.5f ? 1 : -1, h11.Floats01B() > 0.5f ? 1 : -1);

		d00 = glm::vec2(u, v);
		d01 = glm::vec2(u, v - 1);
		d10 = glm::vec2(u - 1, v);
		d11 = glm::vec2(u - 1, v - 1);

		p00 = (g00.x * d00.x + g00.y * d00.y);
		p01 = (g01.x * d01.x + g01.y * d01.y);
		p10 = (g10.x * d10.x + g10.y * d10.y);
		p11 = (g11.x * d11.x + g11.y * d11.y);

		return std::lerp(std::lerp(p00, p10, u), std::lerp(p01, p11, u), v);
	}
};