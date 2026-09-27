/*
The zlib/libpng License
http://opensource.org/licenses/zlib-license.php


Angelscript addon Template Containers
Copyright (c) 2014 Sami Vuorela

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1.	The origin of this software must not be misrepresented;
You must not claim that you wrote the original software.
If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

2.	Altered source versions must be plainly marked as such,
and must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source distribution.


Sami Vuorela
samivuorela@gmail.com
*/



//#include "aatc_internal_lists.hpp"
#include "aatc.hpp"
#include "aatc_common.hpp"
#include "aatc_hash.hpp"
#include "aatc_enginestorage.hpp"
#include "aatc_container_listing.hpp"
#include "aatc_templatemagic.hpp"





BEGIN_AS_NAMESPACE
namespace aatc {



	//used with templatemagic::iterator_functor_1arg to iterate in runtime, over a list of all container types defined in compile time
	template<int i> class staticiterate_initializer_register_all_containers {
	public:
		void operator()(Initializer* initializer, asIScriptEngine* engine) {
			if (initializer->include_container[i]) {
				enginestorage::engine_level_storage* els = enginestorage::Get_ELS(engine);
				enginestorage::containertype_specific_storage& ctss = els->containertype_specific_storages[i];

				ctss.els = els;
				ctss.container_id = i;
				ctss.func_errorcheck_missing_functions_make_bitfield_for_template = container::listing::errorcheck_missing_functions_make_bitfield_for_template<i>;

				container::listing::register_container<i>(engine);
			}
		}
	};



	void Initializer::Go() {
		common::primunion_defaultvalue.ui64 = 0;
		common::primunion_defaultvalue.ptr = nullptr;

		engine->SetUserData(new enginestorage::engine_level_storage(engine), config::detail::engine_userdata_id);
		engine->SetEngineUserDataCleanupCallback(enginestorage::engine_cleanup, config::detail::engine_userdata_id);

		#if aatc_ENABLE_REGISTER_TYPEDEF_HASH_TYPE
			engine->RegisterTypedef(config::scriptname::t::hash, config::scriptname::t::hash_actual);
		#endif

		{
			int r = 0;
			char textbuf[common::RegistrationState::bufsize];

			{//register hash functions
				common::RegistrationState::Format_static(textbuf, 1000, "%s aatc_Hashfunc_djb2(string &in)", config::scriptname::t::hash_actual);
				r = engine->RegisterGlobalFunction(textbuf, asFUNCTION(hash::hashfunc::djb2), asCALL_CDECL); assert(r >= 0);
			}
		}

		templatemagic::staticiterate_2arg<
			0,
			container::listing::CONTAINER::_COUNT - 1,
			staticiterate_initializer_register_all_containers,

			Initializer*,
			asIScriptEngine*
		> f; 
		f(this, engine);
	}


};//namespace aatc
END_AS_NAMESPACE
