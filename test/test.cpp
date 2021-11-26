#include <boost/filesystem.hpp>
#include <gtest/gtest.h>
#include "../src/data.h"
#include "../src/dataslice.h"
namespace {

TEST(Open, txt) {
  // we launch in <root>/build/bin
  // data is in <root>/data
  std::string path = "../../data/ecg.txt";
  if(!boost::filesystem::exists(path))
    path = "../data/ecg.txt";
  if(!boost::filesystem::exists(path))
    path = "data/ecg.txt";

  EXPECT_TRUE(boost::filesystem::exists(path));
  if(boost::filesystem::exists(path))
  {
    sc::Model mdl{path};
    mdl.normalizationrange = 0.f;   // no normalization 
    auto data = sc::open(mdl);

    EXPECT_EQ(data.raw.size(), data.normed.size());
    EXPECT_EQ(data.raw.size(), 12254lu);
    EXPECT_EQ(data.raw.front().time, 0.f);
    EXPECT_NEAR(data.raw.back().time, 12254lu / 500.f, 1e-3);
    EXPECT_EQ(data.raw.front().value, -444);
    EXPECT_EQ(data.raw.back().value, -5151);
  }
}

TEST(Open, 7z) {
  // TODO
}

TEST(Data, normalize_0) {
  sc::Data data;
  for(auto i = 1lu; i < 11lu; ++i)
    data.raw.push_back(sc::Point{float(i), float(i)});

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
  for(auto i = 1lu; i < 11lu; ++i)
    data.raw.push_back(sc::Point{float(i), float(i)});

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
  for(auto i = 1lu; i < 7lu; ++i)
    data.raw.push_back(sc::Point{float(i), float(i*i)});
  data.normalize(3.1);
  EXPECT_EQ(data.raw.size(), data.normed.size());
  EXPECT_EQ(data.raw.size(), 6lu);
  for(auto i = 0lu; i < 6lu; ++i)
    EXPECT_EQ(data.raw[i].time, data.normed[i].time);

  EXPECT_EQ(data.normed[0].value, -1.5f);
  for(auto i = 1lu; i < 5lu; ++i)
    EXPECT_NEAR(data.normed[i].value, -2.f/3.f, 1e-4);
  EXPECT_EQ(data.normed[5].value, 5.5f);
}

TEST(Data, dataslice_centerperiod) {
  sc::Model mdl{std::string("")};
  mdl.rate = 1.f;
  mdl.windowrate = 1.f;
  mdl.refreshrate = 2.f;
  EXPECT_NEAR(sc::DataSlice::centerperiod(mdl), 0.5f, 1e-5f);

  mdl.rate = 2.f;
  mdl.windowrate = 1.f;
  mdl.refreshrate = 1.f;
  EXPECT_NEAR(sc::DataSlice::centerperiod(mdl), 0.5f, 1e-5f);

  mdl.rate = 1.f;
  mdl.windowrate = 2.f;
  mdl.refreshrate = 1.f;
  EXPECT_NEAR(sc::DataSlice::centerperiod(mdl), 2.f, 1e-5f);
}

TEST(DataSlice, equal)
{
  {
    sc::DataSlice left{0lu, 1lu, 1.f}, right{0lu, 1lu, 1.f};
    EXPECT_EQ(left, right);
  }

  {
    sc::DataSlice left{0lu, 0lu, 1.f}, right{0lu, 1lu, 1.f};
    EXPECT_EQ(left == right, false);
  }

  {
    sc::DataSlice left{1lu, 2lu, 1.f}, right{1lu, 1lu, 1.f};
    EXPECT_EQ(left == right, false);
  }

  {
    sc::DataSlice left{1lu, 2lu, 1.f}, right{1lu, 2lu, 1.00001f};
    EXPECT_EQ(left == right, false);
  }
}

TEST(DataSlice, refresh)
{
  sc::Model model{std::string(""), 1.f, 3.f, 1.f, 1.f, 0.f};
  sc::Data data;
  for(auto i = 1lu; i < 8lu; ++i)
    data.raw.push_back(sc::Point{float(i), float(i)});
  data.normalize(0.f);

  sc::DataSlice slice{};
  slice = slice.refresh(model, data);
  EXPECT_EQ(slice.begin, 0lu);
  EXPECT_EQ(slice.end, 2lu);
  EXPECT_EQ(slice.center, 1.f);

  slice = slice.refresh(model, data);
  EXPECT_EQ(slice.begin, 0lu);
  EXPECT_EQ(slice.end, 3lu);
  EXPECT_EQ(slice.center, 2.f);

  slice = slice.refresh(model, data);
  EXPECT_EQ(slice.begin, 1lu);
  EXPECT_EQ(slice.end, 4lu);
  EXPECT_EQ(slice.center, 3.f);

  slice = slice.refresh(model, data);
  EXPECT_EQ(slice.begin, 2lu);
  EXPECT_EQ(slice.end, 5lu);
  EXPECT_EQ(slice.center, 4.f);

  slice = slice.refresh(model, data);
  EXPECT_EQ(slice.begin, 3lu);
  EXPECT_EQ(slice.end, 6lu);
  EXPECT_EQ(slice.center, 5.f);

  slice = slice.refresh(model, data);
  EXPECT_EQ(slice.begin, 4lu);
  EXPECT_EQ(slice.end, 8lu);
  EXPECT_EQ(slice.center, 6.f);
}
}
