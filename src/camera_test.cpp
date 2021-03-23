#include "gtest/gtest.h"

#include <cstdio>
#include "camera.h"

TEST(CameraTest, Test1) {
  Camera camera(1.0f, 1.0f, 1.0f);
  camera.translator.pos.x = 0;
  camera.translator.pos.y = 0;
  camera.translator.pos.z = 0;
  camera.process_input(Camera::FORWARD, 10.0f);
  EXPECT_EQ(camera.translator.pos.z, -10.0f);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
