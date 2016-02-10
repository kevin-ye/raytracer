#pragma once

enum class MaterialType {
	PhongMaterial
};

class Material {
public:
  virtual ~Material();

  virtual MaterialType getMaterialType() = 0;

protected:
  Material();
};
