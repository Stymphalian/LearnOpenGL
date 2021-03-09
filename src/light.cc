#include "light.h"
#include "misc.h"
#include "shader.h"

void Light::use(Shader *shader, int lightIndex) {
  shader->use();
  if (type == POINT) {
    useAsPoint(shader, lightIndex);
  } else if (type == SPOT) {
    useAsSpot(shader, lightIndex);
  } else if (type == DIRECTIONAL) {
    useAsDirectional(shader, lightIndex);
  }
}

void Light::useAsPoint(Shader *shader, int lightIndex) {
  shader->set3Float(string_format("pointLights[%d].position", lightIndex),
                    getPosition());
  shader->set3Float(string_format("pointLights[%d].light.ambient", lightIndex),
                    ambient);
  shader->set3Float(string_format("pointLights[%d].light.diffuse", lightIndex),
                    diffuse);
  shader->set3Float(string_format("pointLights[%d].light.specular", lightIndex),
                    specular);
  shader->setFloat(
      string_format("pointLights[%d].attenuation.constant", lightIndex),
      constant);
  shader->setFloat(
      string_format("pointLights[%d].attenuation.linear", lightIndex), linear);
  shader->setFloat(
      string_format("pointLights[%d].attenuation.quadratic", lightIndex),
      quadratic);
}
void Light::useAsDirectional(Shader *shader, int lightIndex) {
  shader->set3Float(
      string_format("directionalLights[%d].direction", lightIndex), getDirection());
  shader->set3Float(
      string_format("directionalLights[%d].light.ambient", lightIndex),
      ambient);
  shader->set3Float(
      string_format("directionalLights[%d].light.diffuse", lightIndex),
      diffuse);
  shader->set3Float(
      string_format("directionalLights[%d].light.specular", lightIndex),
      specular);
}

void Light::useAsSpot(Shader *shader, int lightIndex) {
  shader->set3Float(string_format("spotLights[%d].point.position", lightIndex),
                    getPosition());
  shader->set3Float(
      string_format("spotLights[%d].point.light.ambient", lightIndex), ambient);
  shader->set3Float(
      string_format("spotLights[%d].point.light.diffuse", lightIndex), diffuse);
  shader->set3Float(
      string_format("spotLights[%d].point.light.specular", lightIndex),
      specular);
  shader->setFloat(
      string_format("spotLights[%d].point.attenuation.constant", lightIndex),
      constant);
  shader->setFloat(
      string_format("spotLights[%d].point.attenuation.linear", lightIndex),
      linear);
  shader->setFloat(
      string_format("spotLights[%d].point.attenuation.quadratic", lightIndex),
      quadratic);
  shader->set3Float(string_format("spotLights[%d].direction", lightIndex),
                    getDirection());
  shader->setFloat(string_format("spotLights[%d].innerCutoff", lightIndex),
                   innerCutoff);
  shader->setFloat(string_format("spotLights[%d].outerCutoff", lightIndex),
                   outerCutoff);
}
