#include "o3d3xx.h"
#include <cstdint>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "gtest/gtest.h"

class DenseImageTest : public ::testing::Test
{
protected:
  virtual void SetUp()
  {
    cam_ = std::make_shared<o3d3xx::Camera>();
    cam_->RequestSession();
    cam_->SetOperatingMode(o3d3xx::Camera::operating_mode::EDIT);

    dev_ = cam_->GetDeviceConfig();
    old_active_idx_ = dev_->ActiveApplication();

    std::string test_dir(__FILE__);
    test_dir.erase(test_dir.find_last_of("/") + 1);
    std::string in = test_dir + infile_;

    std::ifstream ifs(in, std::ios::in|std::ios::binary);
    ifs.unsetf(std::ios::skipws);
    std::streampos file_size;
    ifs.seekg(0, std::ios::end);
    file_size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<std::uint8_t> bytes;
    bytes.reserve(file_size);

    bytes.insert(bytes.begin(),
                 std::istream_iterator<std::uint8_t>(ifs),
                 std::istream_iterator<std::uint8_t>());

    idx_ = cam_->ImportIFMApp(bytes);
    dev_->SetActiveApplication(idx_);
    cam_->SetDeviceConfig(dev_.get());
    cam_->SaveDevice();
    cam_->CancelSession();
  }

  virtual void TearDown()
  {
    cam_->RequestSession();
    cam_->SetOperatingMode(o3d3xx::Camera::operating_mode::EDIT);
    dev_->SetActiveApplication(old_active_idx_);
    cam_->SetDeviceConfig(dev_.get());
    cam_->SaveDevice();
    cam_->DeleteApplication(idx_);
  }

  o3d3xx::Camera::Ptr cam_;
  o3d3xx::DeviceConfig::Ptr dev_;
  std::string infile_ = "data/100k.o3d3xxapp";
  int idx_ = -1;
  int old_active_idx_ = -1;
};

TEST_F(DenseImageTest, ActiveApplication)
{
  ASSERT_EQ(idx_, dev_->ActiveApplication());
}

TEST_F(DenseImageTest, ImageSize)
{
  //
  //! @todo: Once the JSON is fixed on the camera,
  //! we should get the expected height/width of the
  //! image from the XML-RPC interface.
  //
  int N_ROWS = 264; // height
  int N_COLS = 352; // width

  o3d3xx::FrameGrabber::Ptr fg =
    std::make_shared<o3d3xx::FrameGrabber>(cam_);

  o3d3xx::ImageBuffer::Ptr img =
    std::make_shared<o3d3xx::ImageBuffer>();

  pcl::PointCloud<o3d3xx::PointT>::Ptr cloud;
  cv::Mat dimg;
  cv::Mat aimg;
  cv::Mat cimg;

  // put this in a loop in case we want to tests against multiple image grabs.
  int i = 0;
  while (i < 1)
    {
      EXPECT_TRUE(fg->WaitForFrame(img.get(), 2000));

      cloud = img->Cloud();
      dimg = img->DepthImage();
      aimg = img->AmplitudeImage();
      cimg = img->ConfidenceImage();

      ASSERT_EQ(cloud->width, N_COLS);
      ASSERT_EQ(cloud->height, N_ROWS);

      ASSERT_EQ(dimg.size().width, N_COLS);
      ASSERT_EQ(dimg.size().height, N_ROWS);

      ASSERT_EQ(aimg.size().width, N_COLS);
      ASSERT_EQ(aimg.size().height, N_ROWS);

      ASSERT_EQ(cimg.size().width, N_COLS);
      ASSERT_EQ(cimg.size().height, N_ROWS);

      i++;
    }

  EXPECT_EQ(i, 1);
  fg->Stop();
  EXPECT_FALSE(fg->WaitForFrame(img.get(), 500));
}
