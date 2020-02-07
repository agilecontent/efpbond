//
// UnitX Edgeware AB 2020
//

//Methods are NOT thread safe meaning you may only call all methods from the same thread as calling distributeDataGroup

// Prefixes used
// m class member
// p pointer (*)
// r reference (&)
// h part of header
// l local scope

#ifndef EFPBOND__EFPBONDING_H
#define EFPBOND__EFPBONDING_H

#include <vector>
#include <iostream>
#include <tuple>

#define EFP_BONDING_MAJOR_VERSION 0
#define EFP_BONDING_MINOR_VERSION 1

#define EFP_MASTER_INTERFACE true
#define EFP_NORMAL_INTERFACE false


enum class EFPBondingMessages : int16_t {
  interfaceIDNotFound = -10000, // Interface ID not found
  removeGroupNotFound,          // Group ID not found
  noGroupsFound,                // The list of groups is empty
  parameterError,               // The parameters given when changing the commit for the interfaces are out of spec.
  noError = 0,                  // No error
  coverageNot100Percent         // Warning payload under 100%
};

class EFPBonding {
public:

  ///@EFPBondingInterfaceID unique id of the single interface used by the EFPBonding class
  typedef uint64_t EFPBondingInterfaceID;

  ///@EFPBondingGroupID unique id of the group interface used by the EFPBonding class
  typedef uint64_t EFPBondingGroupID;

  ///EFPStatistics
  ///@noGapsCoveredFor number of fragments this interface has covered for
  ///@noFragmentsSent fragments sent by this interface
  ///@percentOfTotalTraffic % of total traffic sent by EFPBonding has been sent by this interface
  class EFPStatistics {
  public:
    uint64_t mNoGapsCoveredFor = 0;
    uint64_t mNoFragmentsSent = 0;
    double mPercentOfTotalTraffic = 0;
  };

  ///EFPInterface
  ///@mInterfaceID The unique ID of this interface
  ///@mFireCounter The fragment counter for this interface (The commit counter)
  ///@mFragmentCounter Number of fragments sent trough this interface
  ///@mForwardMissingFragment The number of fragments sent trough this interface due to calculation errors (Debug.. keep for now)
  ///@mInterfaceLocation The location of the interface callback
  ///@mMasterInterface If this is the master interface (Debug... keep for now)
  ///@mCommit % of total traffic this interface committed to.
  class EFPInterface {
  public:
    EFPBondingInterfaceID mInterfaceID = 0;
    double mFireCounter = 0;
    uint64_t mFragmentCounter;
    uint64_t mForwardMissingFragment; //For debug purpose
    std::function<void(const std::vector<uint8_t> &)> mInterfaceLocation = nullptr;
    bool mMasterInterface = EFP_NORMAL_INTERFACE;  //For debug purpose
    double mCommit = 0;
  };

  ///EFPGroup
  ///@mGroupID Group ID
  ///@mGroupInterfaces The list of interfaces belonging to this group
  class EFPGroup {
  public:
    EFPBondingGroupID mGroupID = 0;
    std::vector<std::shared_ptr<EFPInterface>> mGroupInterfaces;
  };

  ///EFPInterfaceCommit
  ///@mCommit commit value %
  ///@mInterfaceID The interface ID
  ///@mGroupID The group ID
  class EFPInterfaceCommit {
  public:
    double mCommit = 0;
    EFPBonding::EFPBondingInterfaceID mInterfaceID = 0;
    EFPBonding::EFPBondingGroupID mGroupID = 0;
  };

  ///Constructor
  explicit EFPBonding();

  ///Destructor
  virtual ~EFPBonding();

  ///Delete copy and move constructors and assign operators
  EFPBonding(EFPBonding const &) = delete;              // Copy construct
  EFPBonding(EFPBonding &&) = delete;                   // Move construct
  EFPBonding &operator=(EFPBonding const &) = delete;   // Copy assign
  EFPBonding &operator=(EFPBonding &&) = delete;        // Move assign

  ///Return the version of the current implementation
  uint16_t getVersion() { return (EFP_BONDING_MAJOR_VERSION << 8) | EFP_BONDING_MINOR_VERSION; }

  ///Returns the statistics fo a interface
  ///@interfaceID the ID of the interface to get statistics for
  ///@groupID the ID of the group to get statistics for if not provided the statistics will be from a interface not belonging to a group.
  ///@reset resets the packet counter for the interface
  EFPStatistics getStatistics(EFPBonding::EFPBondingInterfaceID interfaceID, EFPBonding::EFPBondingGroupID groupID, bool reset);

  ///Modify a interface commit level
  ///@rInterfaceCommit The new % commit for the interface specified in EFPInterfaceCommit
  EFPBondingMessages modifyInterfaceCommit(EFPBonding::EFPInterfaceCommit &rInterfaceCommit);

  ///Modify all interfaces commit level for a group
  ///@rInterfacesCommit a vector od new % commits for the interfaces specified in EFPInterfaceCommit
  EFPBondingMessages modifyTotalGroupCommit(std::vector<EFPBonding::EFPInterfaceCommit> &rInterfacesCommit);

  ///Returns the total number of fragments dealt with by EFPBonding
  uint64_t getGlobalPacketCounter();

  ///re-sets the globsal packet counter
  void clearGlobalPacketCounter();

  ///Returns a unique interfaceID
  EFPBondingInterfaceID generateInterfaceID();

  ///Adds a group of interfaces to EFPBonding
  ///@rInterfaces A list/vector of EFPInterfaces to be grouped together
  EFPBondingGroupID addInterfaceGroup(std::vector<EFPInterface> &rInterfaces);

  ///Distributes the data to all groups registerd
  ///@rSubPacket the data to be distributed
  EFPBondingMessages distributeDataGroup(const std::vector<uint8_t> &rSubPacket);

  ///Removes a interface group from EFPBonding
  ///@groupID the ID of the group to remove
  EFPBondingMessages removeGroup(EFPBondingGroupID groupID);

private:
  
  uint64_t mGlobalPacketCounter = 0;
  //FIXME mMonotonicPacketCounter is not monotonic since we have to reset when adding new groups..
  //FIXME fix how to deal with adding and deleting groups.
  uint64_t mMonotonicPacketCounter = 0;
  EFPBondingInterfaceID mUniqueInterfaceID = 1;
  EFPBondingGroupID mUniqueGroupID = 1;
  std::vector<EFPGroup> mGroupList;

};

#endif //EFPBOND__EFPBONDING_H
