#include <cmath>

#include "image.h"
#include "pinhole-camera.h"
#include "scene.h"
#define GRAY vec3f(0.1f, 0.1f, 0.1f)
#define BLACK vec3f(0);
#define White vec3f(1);

// 光源の方向
const Vec3f LIGHT_DIRECTION = normalize(Vec3f(0.5, 1, 0.5));

<<<<<<< HEAD
// 反射ベクトルの計算
Vec3f reflect(const Vec3f &v, const Vec3f &n)
{
  return -v + 2.0f * dot(v, n) * n;
}

// 屈折ベクトルの計算
Vec3f refract(const Vec3f &v, const Vec3f &n, float ior1, float ior2)
{
  return Vec3f(0);
}

=======
>>>>>>> 8660984dacb76ee330e572a4c65c33383a359efb
// 古典的レイトレーシングの処理
Vec3f raytrace(const Ray &ray_in, const Scene &scene)
{
  constexpr int MAX_DEPTH = 100; // 再帰の最大深さ

  Ray ray = ray_in;
  Vec3f color(0); // 最終的な色

  IntersectInfo info;
<<<<<<< HEAD
  for (int i = 0; i < MAX_DEPTH; ++i)
  {
    if (scene.intersect(ray, info))
    {
      if (info.hitSphere->material_type == MaterialType::Mirror)
      {
        // 次のレイをセット
        ray = Ray(info.hitPos, reflect(-ray.direction, info.hitNormal));
      }
      else if (info.hitSphere->material_type == MaterialType::Glass)
      {
=======
  for (int i = 0; i < MAX_DEPTH; ++i) {
    if (scene.intersect(ray, info)) {
      // ミラーの場合
      if (info.hitSphere->material_type == MaterialType::Mirror) {
        // 次のレイをセット
        ray = Ray(info.hitPos, reflect(-ray.direction, info.hitNormal));
      }
      // ガラスの場合
      else if (info.hitSphere->material_type == MaterialType::Glass) {
>>>>>>> 8660984dacb76ee330e572a4c65c33383a359efb
        // 球の内部にあるか判定
        bool is_inside = dot(-ray.direction, info.hitNormal) < 0;

        // 次のレイの方向の計算
        Vec3f next_direction;
<<<<<<< HEAD
        if (!is_inside)
        {
          next_direction = refract(-ray.direction, info.hitNormal, 1.0f, 1.5f);
        }
        else
        {
          next_direction = refract(-ray.direction, -info.hitNormal, 1.5f, 1.0f);
=======
        if (!is_inside) {
          if (!refract(-ray.direction, info.hitNormal, 1.0f, 1.5f,
                       next_direction)) {
            next_direction = reflect(-ray.direction, info.hitNormal);
          }
        } else {
          if (!refract(-ray.direction, -info.hitNormal, 1.5f, 1.0f,
                       next_direction)) {
            next_direction = reflect(-ray.direction, -info.hitNormal);
          }
>>>>>>> 8660984dacb76ee330e572a4c65c33383a359efb
        }

        // 次のレイをセット
        ray = Ray(info.hitPos, next_direction);
      }
<<<<<<< HEAD
      else
      {
        // 光源が見えるかテスト
        Ray shadow_ray(info.hitPos, LIGHT_DIRECTION);
        IntersectInfo shadow_info;
        if (!scene.intersect(shadow_ray, shadow_info))
        {
          // 光源が見えたら寄与を計算
          color = std::max(dot(LIGHT_DIRECTION, info.hitNormal), 0.0f) *
                  info.hitSphere->kd;
        }
        else
        {
          // 光源から見えなかったら一定量の寄与を与える
=======
      // その他の場合
      else {
        // 光源が見えるかテスト
        Ray shadow_ray(info.hitPos, LIGHT_DIRECTION);
        IntersectInfo shadow_info;

        // 光源が見える場合
        if (!scene.intersect(shadow_ray, shadow_info)) {
          color = std::max(dot(LIGHT_DIRECTION, info.hitNormal), 0.0f) *
                      info.hitSphere->kd +
                  Vec3f(0.1f) * info.hitSphere->kd;
        }
        // 光源が見えない場合
        else {
>>>>>>> 8660984dacb76ee330e572a4c65c33383a359efb
          color = Vec3f(0.1f) * info.hitSphere->kd;
        }
      }
    }
    else
    {
      break;
    }
  }

  return color;
}

int main()
{
  constexpr int width = 512;
  constexpr int height = 512;
  Image img(width, height);

  const Vec3f camPos(4, 1, 7);
  const Vec3f lookAt(0);
  PinholeCamera camera(camPos, normalize(lookAt - camPos));

  // シーンの作成
  Scene scene;
  scene.addSphere(
      Sphere(Vec3f(0, -1001, 0), 1000.0, Vec3f(0.9), MaterialType::Diffuse));
  scene.addSphere(Sphere(Vec3f(-2, 0, 1), 1.0, Vec3f(0.8, 0.2, 0.2),
                         MaterialType::Diffuse));
  scene.addSphere(
      Sphere(Vec3f(0), 1.0, Vec3f(0.2, 0.8, 0.2), MaterialType::Diffuse));
  scene.addSphere(Sphere(Vec3f(2, 0, -1), 1.0, Vec3f(0.2, 0.2, 0.8),
                         MaterialType::Diffuse));
  scene.addSphere(Sphere(Vec3f(-2, 3, 1), 1.0, Vec3f(1), MaterialType::Mirror));
  scene.addSphere(Sphere(Vec3f(3, 1, 2), 1.0, Vec3f(1), MaterialType::Glass));

  for (int j = 0; j < height; ++j)
  {
    for (int i = 0; i < width; ++i)
    {
      // (u, v)の計算
      float u = (2.0f * i - width) / height;
      float v = (2.0f * j - height) / height;

      // レイの生成
      const Ray ray = camera.sampleRay(u, v);

      // 古典的レイトレーシングで色を計算
      img.setPixel(i, j, raytrace(ray, scene));
    }
  }

  // ガンマ補正
  img.gammaCorrection();

  // PPM画像の生成
  img.writePPM("output.ppm");

  return 0;
}