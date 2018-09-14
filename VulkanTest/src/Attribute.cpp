#include <VulkanTest/Attribute.h>
#include <VulkanTest/VulkanManager.h>

VulkanTest::Attribute::Attribute() : num_elements( 0 ) {
}

VulkanTest::Attribute::~Attribute() {
}

uint32_t VulkanTest::Attribute::getNumElements() {
  return num_elements;
}