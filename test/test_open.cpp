#include "gtest/gtest.h"
#include "../src/data.h"
namespace {

TEST(Open, txt) {
  // we launch in <root>/build/bin
  // data is in <root>/data
  sc::Model mdl{std::string("../../data/ecg.txt")};
  mdl.slidingrange = 0.f;   // no normalization 
  auto data = sc::open(mdl);

  EXPECT_EQ(data.raw.size(), data.normed.size());
  EXPECT_EQ(data.raw.size(), 12254lu);
  EXPECT_EQ(data.raw.front().time, 0.f);
  EXPECT_NEAR(data.raw.back().time, 12254lu / 500.f, 1e-3);
  EXPECT_EQ(data.raw.front().value, -444);
  EXPECT_EQ(data.raw.back().value, -5151);
}

TEST(Open, 7z) {
  // TODO
}

TEST(Data, normalize_0) {
  sc::Data data;
  data.raw = {
    sc::Point{1.f, 1.f}, sc::Point{2.f, 2.f},
    sc::Point{3.f, 3.f}, sc::Point{4.f, 4.f},
    sc::Point{5.f, 5.f}, sc::Point{6.f, 6.f},
    sc::Point{7.f, 7.f}, sc::Point{8.f, 8.f},
    sc::Point{9.f, 9.f}, sc::Point{10.f, 10.f},
  };

  data.normalize(0.f);
  EXPECT_EQ(data.raw.size(), data.normed.size());
  EXPECT_EQ(data.raw.size(), 10lu);
  for(auto i = 0lu; i < 10lu; ++i)
  {
    EXPECT_EQ(data.raw[i].time, data.normed[i].time);
    EXPECT_EQ(data.raw[i].value, data.normed[i].value);
  }
}

TEST(Data, normalize_3) {
  sc::Data data;
  data.raw = {
    sc::Point{1.f, 1.f}, sc::Point{2.f, 2.f},
    sc::Point{3.f, 3.f}, sc::Point{4.f, 4.f},
    sc::Point{5.f, 5.f}, sc::Point{6.f, 6.f},
    sc::Point{7.f, 7.f}, sc::Point{8.f, 8.f},
    sc::Point{9.f, 9.f}, sc::Point{10.f, 10.f},
  };
  data.normalize(3.1);
  EXPECT_EQ(data.raw.size(), data.normed.size());
  EXPECT_EQ(data.raw.size(), 10lu);
  for(auto i = 0lu; i < 10lu; ++i)
    EXPECT_EQ(data.raw[i].time, data.normed[i].time);

  EXPECT_EQ(data.normed[0].value, -0.5f);
  for(auto i = 1lu; i < 9lu; ++i)
    EXPECT_NEAR(data.normed[i].value, 0.f, 1e-4);
  EXPECT_EQ(data.normed[9].value, 0.5f);
}

TEST(Data, normalize_square) {
  sc::Data data;
  data.raw = {
    sc::Point{1.f, 1.f},  sc::Point{2.f, 4.f},
    sc::Point{3.f, 9.f},  sc::Point{4.f, 16.f},
    sc::Point{5.f, 25.f}, sc::Point{6.f, 36.f}
  };
  data.normalize(3.1);
  EXPECT_EQ(data.raw.size(), data.normed.size());
  EXPECT_EQ(data.raw.size(), 6lu);
  for(auto i = 0lu; i < 6lu; ++i)
    EXPECT_EQ(data.raw[i].time, data.normed[i].time);

  EXPECT_EQ(data.normed[0].value, -1.5f);
  for(auto i = 1lu; i < 5lu; ++i)
    EXPECT_NEAR(data.normed[i].value, -2.f/3.f, 1e-4);
  EXPECT_EQ(data.normed[9].value, 4.f);
}
}
