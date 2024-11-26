#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
using namespace eosio;
using namespace std;

CONTRACT mycontract : public contract {
    public:
        using contract::contract;


    // Copied from https://github.com/bitcoin/bitcoin
    /** All alphanumeric characters except for "0", "I", "O", and "l" */
    const char* pszBase58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    const int8_t mapBase58[256] = {
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1, 0, 1, 2, 3, 4, 5, 6,  7, 8,-1,-1,-1,-1,-1,-1,
            -1, 9,10,11,12,13,14,15, 16,-1,17,18,19,20,21,-1,
            22,23,24,25,26,27,28,29, 30,31,32,-1,-1,-1,-1,-1,
            -1,33,34,35,36,37,38,39, 40,41,42,43,-1,44,45,46,
            47,48,49,50,51,52,53,54, 55,56,57,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
    };

    bool DecodeBase58(const char* psz, std::vector<unsigned char>& vch)
    {
        // Skip leading spaces.
        while (*psz && isspace(*psz))
            psz++;
        // Skip and count leading '1's.
        int zeroes = 0;
        int length = 0;
        while (*psz == '1') {
            zeroes++;
            psz++;
        }
        // Allocate enough space in big-endian base256 representation.
        int size = strlen(psz) * 733 /1000 + 1; // log(58) / log(256), rounded up.
        std::vector<unsigned char> b256(size);
        // Process the characters.
        static_assert(sizeof(mapBase58)/sizeof(mapBase58[0]) == 256, "mapBase58.size() should be 256"); // guarantee not out of range
        while (*psz && !isspace(*psz)) {
            // Decode base58 character
            int carry = mapBase58[(uint8_t)*psz];
            if (carry == -1)  // Invalid b58 character
                return false;
            int i = 0;
            for (std::vector<unsigned char>::reverse_iterator it = b256.rbegin(); (carry != 0 || i < length) && (it != b256.rend()); ++it, ++i) {
                carry += 58 * (*it);
                *it = carry % 256;
                carry /= 256;
            }
            assert(carry == 0);
            length = i;
            psz++;
        }
        // Skip trailing spaces.
        while (isspace(*psz))
            psz++;
        if (*psz != 0)
            return false;
        // Skip leading zeroes in b256.
        std::vector<unsigned char>::iterator it = b256.begin() + (size - length);
        while (it != b256.end() && *it == 0)
            it++;
        // Copy result into output vector.
        vch.reserve(zeroes + (b256.end() - it));
        vch.assign(zeroes, 0x00);
        while (it != b256.end())
            vch.push_back(*(it++));
        return true;
    }

    bool decode_base58(const string& str, vector<unsigned char>& vch) {
        return DecodeBase58(str.c_str(), vch);
    }

    eosio::public_key stringToPublicKey(string public_key_str){
        string pubkey_prefix("EOS");
        auto result = mismatch(pubkey_prefix.begin(), pubkey_prefix.end(), public_key_str.begin());
        check(result.first == pubkey_prefix.end(), "Public key should be prefix with EOS");
        auto base58substr = public_key_str.substr(pubkey_prefix.length());

        vector<unsigned char> vch;
        check(decode_base58(base58substr, vch), "Decode pubkey failed");
        check(vch.size() == 37, "Invalid public key");

        array<char,33> pubkey_data;
        copy_n(vch.begin(), 33, pubkey_data.begin());

        return eosio::public_key(std::in_place_index<0>, pubkey_data);
    }

	const symbol S_SYS = symbol("EOS", 4);
	const symbol S_RAM = symbol("RAMCORE", 4);

    struct permission_level_weight {
        permission_level  permission;
        uint16_t          weight;
    };

    struct key_weight {
        eosio::public_key  key;
        uint16_t           weight;
    };

    struct wait_weight {
        uint32_t           wait_sec;
        uint16_t           weight;
    };

    struct authority {
        uint32_t                              threshold = 0;
        std::vector<key_weight>               keys;
        std::vector<permission_level_weight>  accounts;
        std::vector<wait_weight>              waits;
    };

    struct newaccount {
        name creator;
        name name;
        authority owner;
        authority active;
    };

    struct rammarket {
        asset    supply;

        struct connector {
            asset balance;
            double weight = .5;
        };

        connector base;
        connector quote;

        uint64_t primary_key()const { return supply.symbol.raw(); }
    };

    typedef eosio::multi_index<"rammarket"_n, rammarket> ramMarket;

    asset getRamCost(uint64_t bytes = 3100){
        ramMarket market(name("eosio"), name("eosio").value);
        auto ramData = market.find(S_RAM.raw());
        check(ramData != market.end(), "Could not get RAM info");

        uint64_t base = ramData->base.balance.amount;
        uint64_t quote = ramData->quote.balance.amount;
        return asset((((double)quote / base))*bytes, S_SYS);
    }

	[[eosio::on_notify("eosio.token::transfer")]]
	void on_transfer(name from, name to, asset quantity, string memo) {
		if(to != get_self()){
			return;
		}

		if(memo == "bypass"){
			return;
		}

		check(quantity.symbol == S_SYS, "Invalid symbol");
		check(quantity.amount > 0, "Invalid amount");

		auto separator = memo.find(":");
		check(separator != string::npos, "Invalid memo format");
		auto account_name_str = memo.substr(0, separator);
		// check that the account name is valid
		check(account_name_str.length() > 0, "Invalid account name");
		check(account_name_str.length() <= 12, "Account name is too long");

		// check that the public key is valid
		auto public_key_str = memo.substr(separator + 1);
		check(public_key_str.length() == 53, "Public key is too long");

		auto account = name(account_name_str);
		auto public_key = stringToPublicKey(public_key_str);

		check(!is_account(account), "Account already exists");

		key_weight k = key_weight{public_key, 1};
        authority auth{
            .threshold = 1,
            .keys = {k},
            .accounts = {},
            .waits = {}
        };

		newaccount new_account = newaccount{
	        .creator = _self,
	        .name = account,
	        .owner = auth,
	        .active = auth
	    };

	    action(
	        permission_level{ _self, "active"_n },
	        name("eosio"),
	        name("newaccount"),
	        new_account
	    ).send();

	    asset ram = getRamCost();
	    asset openTokenCost = getRamCost(300);
        asset minimumCost = ram + openTokenCost;


        check(quantity >= minimumCost, "You did not send enough EOS to cover account creation costs");

		// Buy ram for new account
	    action(
	        permission_level{ _self, "active"_n },
	        name("eosio"),
	        name("buyram"),
	        make_tuple(_self, account, ram)
	    ).send();

	    // Buy ram for account opener (used to open balance for user account)
	    action(
	        permission_level{ _self, "active"_n },
	        name("eosio"),
	        name("buyram"),
	        make_tuple(_self, _self, openTokenCost)
	    ).send();

	    // open balance for user account
	    action(
	        permission_level{ _self, "active"_n },
	        name("eosio.token"),
	        name("open"),
	        make_tuple(account, S_SYS, _self)
	    ).send();

	    // send back the remaining EOS
        asset excess = quantity - ram;
        if(excess.amount > 0){
			action(
				permission_level{ _self, "active"_n },
				name("eosio.token"),
				name("transfer"),
				make_tuple(_self, account, excess, string("Overspent on account creation"))
			).send();
		}
	}

	[[eosio::action]] asset estimatecost(){
		asset ram = getRamCost();
        asset openTokenCost = getRamCost(300);
        return ram + openTokenCost;
	}
};