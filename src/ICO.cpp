#include "ICO.hpp"
#include "GenericException.hpp"
#include "Utils.hpp"

#include <cstring>
#include <fstream>

ICO::ICO(std::string filename) {
  std::vector<unsigned char> data =
      Utils::read_file(filename, std::ifstream::binary);
  this->init(data);
}

ICO::ICO(std::vector<unsigned char> &data) { this->init(data); }

void ICO::init(std::vector<unsigned char> &data) {
  unsigned char *ptr = data.data();
  memcpy(&this->header, ptr, sizeof(ICOheader));
  ptr += sizeof(ICOheader);

  for (unsigned int i = 0; i < this->header.number_of_images; i++) {
    ICOdirectory *dir = new ICOdirectory;
    memcpy(dir, ptr, sizeof(ICOdirectory));
    ptr += sizeof(ICOdirectory);
    this->directories.push_back(dir);
  }

  ICOdirectory *first = this->directories[0];
  std::vector<unsigned char> tmp(first->bitmap_size);
  memcpy(tmp.data(), &data.data()[first->offset], first->bitmap_size);
  this->image = Image::load(tmp);
  /*
  for(ICOdirectory *dir : this->directories) {
  }
  */
}

ICO::~ICO() {
  for (ICOdirectory *dir : this->directories) {
    delete dir;
  }
  delete this->image;
  /*
  for(Image *img : this->images) {
  }
  */
}

Vector2<int> ICO::get_size() const { return this->image->get_size(); }

unsigned int ICO::get_bytes_per_pixel() const {
  return this->image->get_bytes_per_pixel();
}

unsigned char *ICO::get_pixels() const { return this->image->get_pixels(); }

bool ICO::is_valid(unsigned char *bytes) {
  if (memcmp(bytes, ICO::SIGNATURE.data(), ICO::SIGNATURE.size()) == 0) {
    return true;
  }
  return false;
}