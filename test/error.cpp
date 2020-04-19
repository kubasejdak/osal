/// modification, are permitted provided that the following conditions are met:
///
/// 1. Redistributions of source code must retain the above copyright notice, this
///    list of conditions and the following disclaimer.
///
/// 2. Redistributions in binary form must reproduce the above copyright notice,
///    this list of conditions and the following disclaimer in the documentation
///    and/or other materials provided with the distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
/// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
/// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
/// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
/// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
/// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
/// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
/// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/////////////////////////////////////////////////////////////////////////////////////

#include <osal/error.hpp>

#include <catch2/catch.hpp>

#include <string_view>
#include <system_error>

TEST_CASE("Errors have proper human readable messages", "[unit][cpp][error]")
{
    const std::string_view cUnrecognizedMsg = "(unrecognized error)";
    constexpr int cErrorsCount = 8;

    for (int i = 0; i < cErrorsCount; ++i) {
        std::error_code error = static_cast<OsalError>(i);
        REQUIRE(std::string_view(error.category().name()) == "osal");
        REQUIRE(!error.message().empty());
        REQUIRE(error.message() != cUnrecognizedMsg);
    }

    constexpr int cInvalidError = 999;
    std::error_code error = static_cast<OsalError>(cInvalidError);
    REQUIRE(std::string_view(error.category().name()) == "osal");
    REQUIRE(!error.message().empty());
    REQUIRE(error.message() == cUnrecognizedMsg);
}
