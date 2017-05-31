
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Filesystems/Nfs.h"
#include "storage/Filesystems/MountPoint.h"

#include "testsuite/CompoundAction/Fixture.h"

using namespace storage;


BOOST_FIXTURE_TEST_SUITE(nfs_sentence, test::CompoundActionFixture)

BOOST_AUTO_TEST_CASE(test_sentence_on_creating)
{
    initialize_staging_with_two_partitions();

    auto nfs = Nfs::create(staging, "192.168.0.1", "/dir"); 
    nfs->create_mount_point("/test");

    auto actiongraph = storage->calculate_actiongraph();
    
    auto compound_action = find_compound_action_by_target(actiongraph, nfs);
    
    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Mount NFS 192.168.0.1:/dir on /test");
}


BOOST_AUTO_TEST_CASE(test_sentence_on_deleting)
{
    initialize_with_devicegraph("devicegraph.xml");

    auto nfs = Nfs::get_all(staging)[0];
    nfs->remove_descendants();

    auto actiongraph = storage->calculate_actiongraph();
 
    auto probed_nfs = Nfs::get_all(storage->get_probed())[0];
    auto compound_action = find_compound_action_by_target(actiongraph, probed_nfs);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Unmount NFS 192.168.0.1:/dir at /test");
}

BOOST_AUTO_TEST_SUITE_END()

