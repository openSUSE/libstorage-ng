
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/LvmVg.h"
#include "testsuite/CompoundAction/Fixture.h"

using namespace storage;


BOOST_FIXTURE_TEST_SUITE(lvm_vg_sentence, test::CompoundActionFixture)


BOOST_AUTO_TEST_CASE(test_sentence_on_creating)
{
    initialize_staging_with_two_partitions();

    auto vg = LvmVg::create(staging, "vg-name");
    vg->add_lvm_pv(sda2);

    auto actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, vg);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Create volume group vg-name (496.00 MiB) with /dev/sda2 (500.00 MiB)");
}


BOOST_AUTO_TEST_SUITE_END()
