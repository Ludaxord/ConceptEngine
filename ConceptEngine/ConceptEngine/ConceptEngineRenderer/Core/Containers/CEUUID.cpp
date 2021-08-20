#include "CEUUID.h"

static std::random_device RandomDevice;
static std::mt19937_64 Eng(RandomDevice());
static std::uniform_int_distribution<uint64_t> UniformDistribution;

CEUUID::CEUUID(): UUID(UniformDistribution(Eng)) {
}

CEUUID::CEUUID(uint64 uuid) : UUID(uuid) {
}

CEUUID::CEUUID(const CEUUID& uuid): UUID(uuid.UUID) {
}
