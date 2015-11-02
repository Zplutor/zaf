#include <zaf/internal/theme/theme.h>

namespace zaf {
namespace internal {
namespace theme {

std::shared_ptr<Theme> g_theme;

std::shared_ptr<Theme> GetTheme() {
	return g_theme;
}

void SetTheme(const std::shared_ptr<Theme>& theme) {
	g_theme = theme;
}

}
}
}