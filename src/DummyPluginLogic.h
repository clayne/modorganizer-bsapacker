#ifndef DUMMYPLUGINLOGIC_H
#define DUMMYPLUGINLOGIC_H

#include <bsapacker/IDummyPluginLogic.h>
#include <bsapacker/ISettingsService.h>
#include <bsapacker/IArchiveExtensionService.h>

namespace BsaPacker
{
	class DummyPluginLogic : public IDummyPluginLogic
	{
	public:
		DummyPluginLogic(const ISettingsService* settingsService,
						 const IArchiveExtensionService* archiveExtensionService);
		~DummyPluginLogic() override = default;
		[[nodiscard]] bool canCreateDummyESP(const QString& fileNameNoExtension) const override;
		[[nodiscard]] bool canCreateDummyESL(const QString& fileNameNoExtension) const override;

	private:
		template<std::size_t SIZE>
		bool canCreateDummy(const std::array<QString, SIZE>& fileList,
							const QString& fileNameNoExtension) const;

		const ISettingsService* m_SettingsService = nullptr;
		const IArchiveExtensionService* m_ArchiveExtensionService = nullptr;
	};
}

#endif // DUMMYPLUGINLOGIC_H