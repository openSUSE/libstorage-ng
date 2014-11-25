//
// Python SWIG interface definition for libstorage
//

%include "../storage.i"

%inline %{

    const storage::Partition* CastToPartition(const storage::Device* device) {
	return dynamic_cast<const storage::Partition*>(device);
    }

    const storage::PartitionTable* CastToPartitionTable(const storage::Device* device) {
	return dynamic_cast<const storage::PartitionTable*>(device);
    }

%}
