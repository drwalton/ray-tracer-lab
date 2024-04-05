#include <Eigen/Dense>
#include <tgaimage.h>
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Triangle.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "PointLight.hpp"
#include "DirectionalLight.hpp"
#include "LambertianShader.hpp"
#include "TexturedLambertianShader.hpp"
#include "PhongShader.hpp"
#include "MirrorShader.hpp"
#include "Model.hpp"
#include "AABBMesh.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

int main(int argc, char* argv[]) {

	std::cout << refract(Eigen::Vector3f(1.f, 0.1f, 0.f).normalized(), Eigen::Vector3f(1.f, 0.f, 0.f).normalized(), 1.4f) << std::endl;

	const int pixWidth = 1920;
	const int pixHeight = 1080;
	//const int pixWidth = 640;
	//const int pixHeight = 480;
	const int maxBounces = 10;

	TGAColor blackTGA(0,0,0,255);

	Camera cam(Eigen::Vector3f(0.f, 0.f, -5.f),
		Eigen::Vector3f(0.f, 0.f, 1.f),
		Eigen::Vector3f(0.f, 1.f, 0.f),
		pixWidth, pixHeight,
		M_PI * 0.25f);

	TGAImage outImage(pixWidth, pixHeight, TGAImage::RGB);
	TGAImage spotTexture;
	spotTexture.read_tga_file("../models/spot.tga");

	Eigen::Vector3f
		red(1.f, 0.f, 0.f),
		blue(0.f, 0.f, 1.f),
		aqua(0.f, .8f, .8f),
		lavender(178.f / 255.f, 164.f / 255.f, 212.f / 255.f);

	LambertianShader redLambertianShader(red);
	PhongShader bluePlasticShader(blue, Eigen::Vector3f(1.f, 1.f, 1.f), 100.f);
	LambertianShader aquaLambertianShader(aqua);
	LambertianShader lavenderLambertianShader(lavender);
	TexturedLambertianShader spotShader(&spotTexture);
	MirrorShader mirrorShader;

	Scene scene;
	scene.renderables.push_back(std::make_unique<Sphere>(&bluePlasticShader, .8f));
	scene.renderables.back()->modelToWorld(makeTranslationMatrix(Eigen::Vector3f(-2.f, 0.f, 0.f)));

	scene.renderables.push_back(std::make_unique<Sphere>(&mirrorShader, 1.f));
	scene.renderables.back()->modelToWorld(makeTranslationMatrix(Eigen::Vector3f(0.f, 0.f, 0.f)));

	scene.renderables.push_back(std::make_unique<Plane>(&aquaLambertianShader, Eigen::Vector3f(0.f, 0.f, -1.f)));
	scene.renderables.back()->modelToWorld(makeTranslationMatrix(Eigen::Vector3f(0.f, 0.f, 3.f)));

	scene.renderables.push_back(std::make_unique<Plane>(&lavenderLambertianShader, Eigen::Vector3f(0.f, 1.f, 0.f)));
	scene.renderables.back()->modelToWorld(makeTranslationMatrix(Eigen::Vector3f(0.f, -3.f, 0.f)));

	scene.renderables.push_back(std::make_unique<Plane>(&aquaLambertianShader, Eigen::Vector3f(0.f, 0.f, 1.f), VISIBLE_BITMASK));
	scene.renderables.back()->modelToWorld(makeTranslationMatrix(Eigen::Vector3f(0.f, 0.f, -6.f)));

	scene.renderables.push_back(std::make_unique<Triangle>(
		&bluePlasticShader, 
		Eigen::Vector3f(-1.f, 2.f, 1.f),
		Eigen::Vector3f(1.f, 2.f, 1.f),
		Eigen::Vector3f(0.f, 1.5f, 1.f)));
	
	Model spotModel("../models/spot.obj");

	scene.renderables.push_back(std::make_unique<AABBMesh>(
		&spotShader, 
		&spotModel));
	scene.renderables.back()->modelToWorld(
		makeTranslationMatrix(Eigen::Vector3f(2.f, 0.f, 0.f))
		* rotateY(0.f));


	Eigen::Vector3f ambientLight(.1f, .1f, .1f);

	std::vector<std::unique_ptr<Light>> lightSources;
	lightSources.push_back(std::make_unique<PointLight>(Eigen::Vector3f(-1.f, 3.f, -1.f), 3.f * Eigen::Vector3f(1.f, 1.f, 1.f)));
	lightSources.push_back(std::make_unique<DirectionalLight>(Eigen::Vector3f(0.f, -1.f, 1.f), .5f * Eigen::Vector3f(1.f, 1.f, 1.f)));


	// Shuffling the scanline order gets better CPU usage between threads
	// when some lines take longer to render than others.
	std::vector<unsigned int> scanlines(pixHeight);
	for (int i = 0; i < pixHeight; ++i) scanlines[i] = i;
	std::random_device rd;
	std::mt19937 g(rd());
	// Comment out this line to render scanlines in order.
	std::shuffle(scanlines.begin(), scanlines.end(), g);

	auto startTime = std::chrono::steady_clock::now();

	#pragma omp parallel for
	for (int y = 0; y < pixHeight; ++y) {
		for (int x = 0; x < pixWidth; ++x) {
			Ray ray = cam.getRay(x, scanlines[y]);
			HitInfo hitInfo;
			if (scene.intersect(ray, 1e-6f, 1e6f, hitInfo, VISIBLE_BITMASK)) {
				Eigen::Vector3f color = hitInfo.shader->getColor(
					hitInfo, &scene,
					lightSources, ambientLight,
					0, maxBounces);

				color.x() = std::min(color.x(), 1.f);
				color.y() = std::min(color.y(), 1.f);
				color.z() = std::min(color.z(), 1.f);

				TGAColor tgaColor(color.x() * 255, color.y() * 255, color.z() * 255, 255);
				outImage.set(x, scanlines[y], tgaColor);
			}
			else
				outImage.set(x, scanlines[y], blackTGA);
		}
		if (omp_get_thread_num() == omp_get_num_threads()-1) {
			std::clog << "\rScanlines remaining: " << (pixHeight - y) << ' ' << std::flush;
		}

	}

	auto renderTime = std::chrono::steady_clock::now() - startTime;

	std::cout << "Render duration " << std::chrono::duration_cast<std::chrono::seconds>(renderTime).count() << " seconds." << std::endl;

	outImage.flip_vertically();
	outImage.write_tga_file("output.tga");

	return 0;
}
