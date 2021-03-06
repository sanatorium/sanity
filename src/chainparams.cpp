// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2014-2018 The Dash Core developers
// Copyright (c) 2018 The Sanity Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"

#include "arith_uint256.h"
void MineGenesisBlock(const char* infoS, CBlock &genesis);

#include "uint256.h"
#include "arith_uint256.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

 // Build the genesis block. Note that the output of its generation tracaction
 // cannot be spent since it did not originally exist in the database.
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{   // GENGEN
    const char* pszTimestamp = "20180501 BE64D3E018044C66F1FF34704BAF85BF0EB65C5BFA6892D32159BA699768E569";
    // GENGEN mainnetpublickey
    const CScript genesisOutputScript = CScript() << ParseHex("049b1ee46b3d3b5bb75f99a8a6d6bb53d04a749a9af264a8215596c7847946f5a5335a46c20f66118296c2a2e3c43bb68a46ad60cc616250334365e3b9ac25a527") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

void MineGenesisBlock(const char* infoS, CBlock &genesis)
{
    arith_uint256 best = arith_uint256();
    int n=0;
    arith_uint256 hashTarget = arith_uint256().SetCompact(genesis.nBits);
    while (UintToArith256(genesis.GetHash()) > hashTarget)
    {
        arith_uint256 c=UintToArith256(genesis.GetHash());

        if(c < best || n==0)
        {
            best = c;
            n=1;
            // printf("%s %s %s\n",genesis.GetHash().GetHex().c_str(),hashTarget.GetHex().c_str(), best.GetHex().c_str());
        }
        ++genesis.nNonce;
        if (genesis.nNonce == 0) { ++genesis.nTime; }
    }
    printf("MineGenesisBlock for: %s\n", infoS);
    printf("Converting genesis hash to string: %s\n",genesis.ToString().c_str());
}

/**
 * Main network
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nInstantSendKeepLock = 24;

        consensus.nBudgetPaymentsStartBlock = 4070908800; // MODMOD 01/01/2099 -> disabled
        consensus.nBudgetPaymentsCycleBlocks = 21900; // MODMOD actual number of blocks per month is 262800 / 12
        consensus.nBudgetPaymentsWindowBlocks = 100;
        consensus.nBudgetProposalEstablishingTime = 60*60*24;

        consensus.nSuperblockStartBlock = 4070908800; // MODMOD 01/01/2099 -> disabled; The block at which 12.1 goes live (end of final 12.0 budget cycle)
        consensus.nSuperblockCycle = 21900; // MODMOD actual number of blocks per month is 262800 / 12

        consensus.nGovernanceMinQuorum = 10;
        consensus.nGovernanceFilterElements = 20000;

        consensus.nMasternodeMinimumConfirmations = 15; // Masternode input must have at least %d confirmations

        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;

        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256S("0x0"); // MODMOD uint256S("0x000007d91d1254d60e2dd1ae580383070a4ddffa4c64c2eeb4a2f9ecc0414343");

        consensus.powLimit = uint256S("00000fffff000000000000000000000000000000000000000000000000000000");
        consensus.nPowTargetSpacing = 2 * 60; // MODMOD 2 minutes per block
        consensus.nPowTargetTimespan = 1 * 60 * 60; // MODMOD used only in DIFF-BTC (until nPowKGWHeight) and KGW (never) -> unused in DGW -> nTargetTimespan = nPastBlocks * params.nPowTargetSpacing; with nPastBlocks = 24; (ref: pow.cpp)

        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;

        consensus.nPowDGWHeight = 25; // MODMOD DGW from this blockheight, DIFF-BTC before
        consensus.nPowKGWHeight = consensus.nPowDGWHeight; // MODMOD KGW disabled -> nPowKGWHeight >= nPowDGWHeight means "no KGW"

        consensus.nRuleChangeActivationThreshold = 684; // unused default value for rule agreement - was: 1916; // agreement 95% (of 2016) messured across  4 retargeting periods
        consensus.nMinerConfirmationWindow = 720; // unused default value for rule agreement - was: 2016; // nPowTargetTimespan / nPowTargetSpacing

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1486252800; // Feb 5th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1517788800; // Feb 5th, 2018

        // Deployment of DIP0001:  initial scaling mechanism for Dash
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 1508025600; // Oct 15th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 1517788800; // MODMOD Feb 5th, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nWindowSize = 4032;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nThreshold = 3226; // 80% of 4032

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xbe; // MODMOD
        pchMessageStart[1] = 0x1c; // MODMOD
        pchMessageStart[2] = 0x66; // MODMOD
        pchMessageStart[3] = 0xbf; // MODMOD

        // GENGEN valertpublickey
        vAlertPubKey = ParseHex("040c3e84d2abb81ffc46f6f8982cdceed592763609f708ee6d42aecaaf82a2eac6e5677630d1732bd0c29f4a2f293620a82fee34fc490d1f900f008c3b0819cd7a");

        nDefaultPort = 9999; // MODMOD

        nMaxTipAge = 5 * 60 * 60; // MODMOD validation.cpp: IsInitialBlockDownload()? if (chainActive.Tip()->GetBlockTime() < (GetTime() - chainParams.MaxTipAge())) // 150 (was 144 in Dash) blocks behind -> 2 x fork detection time, was 24 * 60 * 60 in bitcoin

        nDelayGetHeadersTime = 24 * 60 * 60;
        nPruneAfterHeight = 1000; // MODMOD (was 10000 in Dash an Bitcoin) disable block pruning on blocks below a certain height

        consensus.defaultAssumeValid = uint256S("0x0"); // IsNull==disabled;  validation.cpp: if (!hashAssumeValid.IsNull()) -> hash of block nr. x (assume valid until this block; 0x0 => Validating signatures for all blocks) // strUsage +=HelpMessageOpt("-assumevalid=<hex>", strprintf(_("If this block is in the chain assume that it and its ancestors are valid and potentially skip their script verification (0 to verify all, default: %s, testnet: %s)"), //Params(CBaseChainParams::MAIN).GetConsensus().defaultAssumeValid.GetHex(), Params(CBaseChainParams::TESTNET).GetConsensus().defaultAssumeValid.GetHex()));

        /*
        MineGenesisBlock for: MAINNET
        Converting genesis hash to string: CBlock(
        hash=00000a609bb4527e1e35162d7a08b3a47df9ae1a479fcd0adf87675936896b60, ver=1,
        hashPrevBlock=0000000000000000000000000000000000000000000000000000000000000000,
        hashMerkleRoot=32b2d01824250c881149c2bbdb12def0efdc80e3685dabd4073a51c966eed30f,
        nTime=1525206118, nBits=1e0ffff0, nNonce=826478, vtx=1)
        CTransaction(hash=32b2d01824, ver=1, vin.size=1, vout.size=1, nLockTime=0)
        CTxIn(COutPoint(0000000000000000000000000000000000000000000000000000000000000000, 4294967295), coinbase 04ffff001d01044932303138303530312042453634443345303138303434433636463146463334373034424146383542463045423635433542464136383932443332313539424136393937363845353639)
        CTxOut(nValue=1.00000000, scriptPubKey=41049b1ee46b3d3b5bb75f99a8a6d6)


        MineGenesisBlock for: TESTNET
        Converting genesis hash to string: CBlock(
        hash=0000096b17ca35b6884cd9269dbacc60b9ab5aa15995b5b8678e6b8a58ba85ca, ver=1,
        hashPrevBlock=0000000000000000000000000000000000000000000000000000000000000000,
        hashMerkleRoot=32b2d01824250c881149c2bbdb12def0efdc80e3685dabd4073a51c966eed30f,
        nTime=1525206167, nBits=1e0ffff0, nNonce=1079926, vtx=1)
        CTransaction(hash=32b2d01824, ver=1, vin.size=1, vout.size=1, nLockTime=0)
        CTxIn(COutPoint(0000000000000000000000000000000000000000000000000000000000000000, 4294967295), coinbase 04ffff001d01044932303138303530312042453634443345303138303434433636463146463334373034424146383542463045423635433542464136383932443332313539424136393937363845353639)
        CTxOut(nValue=1.00000000, scriptPubKey=41049b1ee46b3d3b5bb75f99a8a6d6)


        MineGenesisBlock for: REGTEST
        Converting genesis hash to string: CBlock(
        hash=04d66f13ac867cd69dbf21a1e1fc37337c22698d7604e020ce98630de385c03d, ver=1,
        hashPrevBlock=0000000000000000000000000000000000000000000000000000000000000000,
        hashMerkleRoot=32b2d01824250c881149c2bbdb12def0efdc80e3685dabd4073a51c966eed30f,
        nTime=1525206232, nBits=207fffff, nNonce=0, vtx=1)
        CTransaction(hash=32b2d01824, ver=1, vin.size=1, vout.size=1, nLockTime=0)
        CTxIn(COutPoint(0000000000000000000000000000000000000000000000000000000000000000, 4294967295), coinbase 04ffff001d01044932303138303530312042453634443345303138303434433636463146463334373034424146383542463045423635433542464136383932443332313539424136393937363845353639)
        CTxOut(nValue=1.00000000, scriptPubKey=41049b1ee46b3d3b5bb75f99a8a6d6)
        */

        bool mineGenesis = false;
        // GENGEN
        if (!mineGenesis) {
          // The best chain should have at least this much work.
          // `GET /rest/chaininfo.json` -> * chainwork : (string) total amount of work in active chain, in hexadecimal
          // "getblockheader \"hash\" ( verbose )\n" ->  \"chainwork\" : \"0000...1f3\"     (string) Expected number of hashes required to produce the current chain (in hex)\n"
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          //////////////////////////////////////////////////
          // getblockhash 1250 -> getblock (blockhash) -> chainwork hex:26a1072d4 -> dec:10369397460 -> log2(10369397460) = 33.271613014 == debug.log log2_work
          // debug.log: 2018-05-03 16:24:04 UpdateTip: new best=0000018cc8015783cf2c79c907411a0a993c13999ad1914a14ed8ad0742b5fa8  height=1250  log2_work=33.271613  tx=1251  date=2018-05-03 16:24:04 progress=1.000000  cache=0.1MiB(531txo)
          //////////////////////////////////////////////////
          // getblockhash 12021 -> getblock 00000000481dd2bf20c3721fb867eeb213c8485e9430665d7a597927808c56a7 -> chainwork
          consensus.nMinimumChainWork = uint256S("00000000000000000000000000000000000000000000000000000007660ac8ec"); // was: 0x0 MODMOD validation.cpp: IsInitialBlockDownload()? if (chainActive.Tip()->nChainWork < UintToArith256(chainParams.GetConsensus().nMinimumChainWork)

          genesis = CreateGenesisBlock(1525206118,826478,0x1e0ffff0,1,1*COIN); // nTime, nNonce, nBits, ver
        } else {
          std::time_t unixTimeNow = std::time(nullptr);
          consensus.nMinimumChainWork = uint256S("0x00"); // MODMOD validation.cpp: IsInitialBlockDownload()? if (chainActive.Tip()->nChainWork < UintToArith256(chainParams.GetConsensus().nMinimumChainWork)) // consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000006500074"); // MODMOD

          genesis = CreateGenesisBlock(unixTimeNow,0,0x1e0ffff0,1,1*COIN); // nTime, nNonce, nBits, ver
          MineGenesisBlock("MAINNET", genesis); ///////////////////////////////////////////////////////////////////////////

          unixTimeNow = std::time(nullptr);
          genesis = CreateGenesisBlock(unixTimeNow,0,0x1e0ffff0,1,1*COIN); // nTime, nNonce, nBits, ver
          MineGenesisBlock("TESTNET", genesis); ///////////////////////////////////////////////////////////////////////////

          unixTimeNow = std::time(nullptr);
          genesis = CreateGenesisBlock(unixTimeNow,0,0x207fffff,1,1*COIN); // nTime, nNonce, nBits, ver
          MineGenesisBlock("REGTEST", genesis); ///////////////////////////////////////////////////////////////////////////
        }

        consensus.hashGenesisBlock = genesis.GetHash();

        // GENGEN
        assert(consensus.hashGenesisBlock == uint256S("0x00000a609bb4527e1e35162d7a08b3a47df9ae1a479fcd0adf87675936896b60")); // 0xhash
        assert(genesis.hashMerkleRoot == uint256S("0x32b2d01824250c881149c2bbdb12def0efdc80e3685dabd4073a51c966eed30f")); // 0xhashMerkleRoot

        // MODMOD
        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("seed0.sanity.mn", "seed0.sanity.mn"));
        vSeeds.push_back(CDNSSeedData("seed1.sanity.mn", "seed1.sanity.mn"));
        vSeeds.push_back(CDNSSeedData("seed2.sanity.mn", "seed2.sanity.mn"));
        vSeeds.push_back(CDNSSeedData("seed3.sanity.mn", "seed3.sanity.mn"));
        vSeeds.push_back(CDNSSeedData("seed4.sanity.mn", "seed4.sanity.mn"));
        vSeeds.push_back(CDNSSeedData("seed5.sanity.mn", "seed5.sanity.mn"));
        vSeeds.push_back(CDNSSeedData("seed6.sanity.mn", "seed6.sanity.mn"));
        vSeeds.push_back(CDNSSeedData("seed7.sanity.mn", "seed7.sanity.mn"));
        vSeeds.push_back(CDNSSeedData("seed8.sanity.mn", "seed8.sanity.mn"));
        vSeeds.push_back(CDNSSeedData("seed9.sanity.mn", "seed9.sanity.mn"));

        // MODMOD addresses start with 'S'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,63);
        // MODMOD script addresses start with '8'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,18);
        // MODMOD private keys start with 's'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,125);
        // MODMOD BIP32 pubkeys start with 'xpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        // MODMOD BIP32 prvkeys start with 'xprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        // BIP44 coin type is '5'
        nExtCoinType = 5;

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        nPoolMaxTransactions = 3;
        nFulfilledRequestExpireTime = 60*60; // fulfilled requests expire in 1 hour

        // GENGEN mainnetsporkkey
        strSporkPubKey = "0471215fbac076b1c0cedc690700c800ef38928b823513d8e4de9ca001b4b6bdaa25ac0baa55519e237aaa8b660b68418a13328d297a78d3f7176a7e1d558da70b";

        // GENGEN
        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (    0, consensus.hashGenesisBlock)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //2018-05-02 19:52:56 UpdateTip: new best=000002d0864e20e54953e8dbca38bb0d460b3a0cd942c781037c05e5240889f0  height=666  log2_work=32.273816  tx=667  date=2018-05-02 19:52:56 progress=1.000000  cache=0.1MiB(666txo)
            (  666, uint256S("0x000002d0864e20e54953e8dbca38bb0d460b3a0cd942c781037c05e5240889f0"))
            //2018-05-03 11:28:22 UpdateTip: new best=0000014d113a4b6b18be7df16a480fd1afa5a365526353fd97d8da49eca9b542 height=1111  log2_work=33.090021 tx=1112 date=2018-05-03 11:28:22 (1525346902) progress=1.000000  cache=0.1MiB(392txo)
            ( 1111, uint256S("0x0000014d113a4b6b18be7df16a480fd1afa5a365526353fd97d8da49eca9b542"))
            //2018-05-10 03:20:29 UpdateTip: new best=0000041c72f7dd4406e8b74774dcc82f2c17c6a9b4aa711f01d33730ea2e870e  height=5555  log2_work=34.132525  tx=5571  date=2018-05-10 03:20:26 (1525922426) progress=0.999994  cache=0.3MiB(1825txo)
            ( 5555, uint256S("0x0000041c72f7dd4406e8b74774dcc82f2c17c6a9b4aa711f01d33730ea2e870e"))
            // 2018-05-19 15:12:09 UpdateTip: new best=00000000481dd2bf20c3721fb867eeb213c8485e9430665d7a597927808c56a7  height=12021  log2_work=34.887253  tx=12100  date=2018-05-19 15:12:10 progress=1.000002  cache=0.4MiB(2325txo)
            (12021, uint256S("0x00000000481dd2bf20c3721fb867eeb213c8485e9430665d7a597927808c56a7"))

            ////////////////////////////////////////////////////////
            // last block -> in debug.log -> new best=(blockhash) //
            ////////////////////////////////////////////////////////
            ,
            1526742730, // was: genesis.nTime, // * UNIX timestamp of last checkpoint block -> debug.log -> date=2018-04-08 12:09:05 -> https://www.unixtimestamp.com https://www.epochconverter.com/ -> 1523189345
            12100,      // was: 0, // * total number of transactions between genesis and last checkpoint
                        //   (the tx=... number in the "UpdateTip" debug.log lines)
            1000        // * estimated number of transactions per day after checkpoint
        };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nInstantSendKeepLock = 6;

        consensus.nBudgetPaymentsStartBlock = 4070908800; // MODMOD 01/01/2099 -> disabled
        consensus.nBudgetPaymentsCycleBlocks = 21900; // MODMOD actual number of blocks per month is 262800 / 12
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nBudgetProposalEstablishingTime = 60*20;

        consensus.nSuperblockStartBlock = 4070908800; // MODMOD 01/01/2099 -> disabled; The block at which 12.1 goes live (end of final 12.0 budget cycle)
        consensus.nSuperblockCycle = 21900; // MODMOD actual number of blocks per month is 262800 / 12

        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 500;

        consensus.nMasternodeMinimumConfirmations = 1;

        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 100;

        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256S("0x0"); // MODMOD uint256S("0x0000047d24635e347be3aaaeb66c26be94901a2f962feccd4f95090191f208c1");

        consensus.powLimit = uint256S("00000fffff000000000000000000000000000000000000000000000000000000");
        consensus.nPowTargetSpacing = 2 * 60; // MODMOD 2 minutes
        consensus.nPowTargetTimespan = 1 * 60 * 60;

        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;

        consensus.nPowKGWHeight = 25; // nPowKGWHeight >= nPowDGWHeight means "no KGW"
        consensus.nPowDGWHeight = consensus.nPowKGWHeight;

        consensus.nRuleChangeActivationThreshold = 684; // unused default value - was: 1512; // 75 % for testchains
        consensus.nMinerConfirmationWindow = 720; // unused default value - was: 2016; // nPowTargetTimespan / nPowTargetSpacing

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1486252800; // Feb 5th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1517788800; // Feb 5th, 2018

        // Deployment of DIP0001
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 1508025600; // Oct 15th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 1517788800; // MODMOD Feb 5th, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nWindowSize = 4032;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nThreshold = 3226; // 80% of 4032

        pchMessageStart[0] = 0xcc; // GENGEN
        pchMessageStart[1] = 0xe2;
        pchMessageStart[2] = 0xca;
        pchMessageStart[3] = 0xff;

        // GENGEN publickey
        vAlertPubKey = ParseHex("040c3e84d2abb81ffc46f6f8982cdceed592763609f708ee6d42aecaaf82a2eac6e5677630d1732bd0c29f4a2f293620a82fee34fc490d1f900f008c3b0819cd7a");

        nDefaultPort = 19999;

        nMaxTipAge = 0x7fffffff; // allow mining on top of old blocks for testnet

        nDelayGetHeadersTime = 24 * 60 * 60;
        nPruneAfterHeight = 1000;

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x0"); // MODMOD uint256S("0x0000000004f5aef732d572ff514af99a995702c92e4452c7af10858231668b1f"); // 37900

        bool mineGenesisTestnet = false;
        // GENGEN
        if (!mineGenesisTestnet) {
            // The best chain should have at least this much work.
            consensus.nMinimumChainWork = uint256S("0x0"); // MODMOD uint256S("0x000000000000000000000000000000000000000000000000000924e924a21715"); // 37900

            genesis = CreateGenesisBlock(1525206167,1079926,0x1e0ffff0,1,1*COIN); // nTime, nNonce, nBits, ver
        } else {
            std::time_t unixTimeNow = std::time(nullptr);
            consensus.nMinimumChainWork = uint256S("0x0"); // MODMOD uint256S("0x000000000000000000000000000000000000000000000000000924e924a21715"); // 37900

            genesis = CreateGenesisBlock(unixTimeNow,0,0x1e0ffff0,1,1*COIN); // nTime, nNonce, nBits, ver
            MineGenesisBlock("TESTNET", genesis); ///////////////////////////////////////////////////////////////////////////
        }

        consensus.hashGenesisBlock = genesis.GetHash();

        // GENGEN
        assert(consensus.hashGenesisBlock == uint256S("0x0000096b17ca35b6884cd9269dbacc60b9ab5aa15995b5b8678e6b8a58ba85ca")); // 0xhash
        assert(genesis.hashMerkleRoot == uint256S("0x32b2d01824250c881149c2bbdb12def0efdc80e3685dabd4073a51c966eed30f")); // 0xhashMerkleRoot

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("testseed1.sanity.mn",  "testseed1.sanity.mn"));
        vSeeds.push_back(CDNSSeedData("testseed2.sanity.mn",  "testseed2.sanity.mn"));
        vSeeds.push_back(CDNSSeedData("testseed3.sanity.mn",  "testseed3.sanity.mn"));
        vSeeds.push_back(CDNSSeedData("testseed4.sanity.mn",  "testseed4.sanity.mn"));
        vSeeds.push_back(CDNSSeedData("testseed5.sanity.mn",  "testseed5.sanity.mn"));

        // MODMOD Testnet addresses start with 'T'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,65);
        // MODMOD Testnet script addresses start with '9'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,4);
        // MODMOD Testnet private keys start with 't'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,127);
        // Testnet BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        // Testnet BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        // Testnet BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 3;
        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        // GENGEN testnetsporkkey
        strSporkPubKey = "04e625bc76bb296a1aaddfbbe2313ccd3f4d71d44c09eda93352f49a73bcbf1f974ef40722bbaa2e89a9884b3a87d8cbebb7eae6fcf87f636b07e19335fdf182d5";

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock) // ( 0, uint256S("0x"))
            ,
            genesis.nTime, // * UNIX timestamp of last checkpoint block
            0,          // * total number of transactions between genesis and last checkpoint
            0        // * estimated number of transactions per day after checkpoint
        };
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nInstantSendKeepLock = 6;

        consensus.nBudgetPaymentsStartBlock = 4070908800; // MODMOD 01/01/2099 -> disabled
        consensus.nBudgetPaymentsCycleBlocks = 21900; // MODMOD actual number of blocks per month is 262800 / 12
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nBudgetProposalEstablishingTime = 60*20;

        consensus.nSuperblockStartBlock = 4070908800; // MODMOD 01/01/2099 -> disabled; The block at which 12.1 goes live (end of final 12.0 budget cycle)
        consensus.nSuperblockCycle = 21900; // MODMOD actual number of blocks per month is 262800 / 12

        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 100;

        consensus.nMasternodeMinimumConfirmations = 1;

        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;

        consensus.BIP34Height = -1; // BIP34 has not necessarily activated on regtest
        consensus.BIP34Hash = uint256();

        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetSpacing = 2 * 60; // MODMOD 2 minutes
        consensus.nPowTargetTimespan = 1 * 60 * 60;

        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;

        consensus.nPowKGWHeight = 25; // same as mainnet
        consensus.nPowDGWHeight = consensus.nPowDGWHeight; // same as mainnet

        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;

        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL;

        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 999999999999ULL;

        pchMessageStart[0] = 0xfc;
        pchMessageStart[1] = 0xc1;
        pchMessageStart[2] = 0xb7;
        pchMessageStart[3] = 0xdc;

        nDefaultPort = 19994; // MODMOD

        nMaxTipAge = 5 * 60 * 60; // MODMOD 150 (was 144 in Dash) blocks behind -> 2 x fork detection time, was 24 * 60 * 60 in bitcoin

        nDelayGetHeadersTime = 0; // never delay GETHEADERS in regtests
        nPruneAfterHeight = 1000;

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x0");

        bool mineGenesisRegtest = false;
        // GENGEN
        if (!mineGenesisRegtest) {
            // The best chain should have at least this much work.
            consensus.nMinimumChainWork = uint256S("0x00");

            genesis = CreateGenesisBlock(1525206232,0,0x207fffff,1,1*COIN); // nTime, nNonce, nBits, ver
        } else {
          std::time_t unixTimeNow = std::time(nullptr);
          consensus.nMinimumChainWork = uint256S("0x00");

          genesis = CreateGenesisBlock(unixTimeNow,0,0x207fffff,1,1*COIN); // nTime, nNonce, nBits, ver
          MineGenesisBlock("REGTEST", genesis); ///////////////////////////////////////////////////////////////////////////
        }

        consensus.hashGenesisBlock = genesis.GetHash();

        // GENGEN
        assert(consensus.hashGenesisBlock == uint256S("0x04d66f13ac867cd69dbf21a1e1fc37337c22698d7604e020ce98630de385c03d")); // 0xhash
        assert(genesis.hashMerkleRoot == uint256S("0x32b2d01824250c881149c2bbdb12def0efdc80e3685dabd4073a51c966eed30f")); // 00xhashMerkleRoot

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        // MODMOD Regtest addresses start with 'R'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,60);
        // MODMOD Regtest script addresses start with '7'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,15);
        // MODMOD Regtest private keys start with 'r'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,122);
        // Regtest BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        // Regtest BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        // Regtest BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0x")),
            0,
            0,
            0
        };
   }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}
